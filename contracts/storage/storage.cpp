/**
 *  @file
 *  @copyright defined in cps/LICENSE.txt
 */
#include "storage.hpp"

namespace TOKEN_NAME {
   void store_account( account_name name, const account& account_to_store ) {
      if ( account_to_store.is_empty() ) {
         accounts::remove( account_to_store, name );
      } else {
         accounts::store(account_to_store, name );
      } 
   }

   void apply_storage_transfer( const TOKEN_NAME::transfer& transfer_storage ) {
      cpsio::require_notice( transfer_storage.to, transfer_storage.from );
      cpsio::require_auth( transfer_storage.from );

      account from = get_account( transfer_storage.from );
      account to   = get_account( transfer_storage.to );

      from.balance -= transfer_storage.quantity; /// token subtraction has underflow assertion
      to.balance   += transfer_storage.quantity; /// token addition has overflow assertion

      store_account( transfer_storage.from, from );
      store_account( transfer_storage.to, to );
   }

   bool validate_ipfspath( const char* ipfspath, uint32_t len ) {
      // To be implemented
      return true;  
   }

   bool validate_cpspath( const char* cpspath, uint32_t len ) {
      // To be implemented
      return true;
   }

   uint32_t read_link_from_buffer( const char* buffer, uint32_t bufferlen,
                                TOKEN_NAME::link& link_to_read, uint32_t& cpspathlen, uint32_t ipfspathlen ) {
      // To be implemented
      return 0;
   }
 
   void apply_storage_setlink() {
      TOKEN_NAME::link link_to_set;
      uint32_t cpspathlen;
      uint32_t ipfspathlen;
      char tmp[4098];
      auto bufferlen = read_message(tmp, 4098);
      auto linklen = read_link_from_buffer( tmp, bufferlen, link_to_set, cpspathlen, ipfspathlen );
      cpsio::require_notice( link_to_set.owner );
      cpsio::require_auth( link_to_set.owner );
      validate_ipfspath( link_to_set.ipfspath, ipfspathlen );
      validate_cpspath( link_to_set.cpspath, cpspathlen );
      ::store_str( current_code(), N(storage), link_to_set.cpspath, cpspathlen, (char*)&link_to_set, linklen );
   }
   
   void apply_storage_removelink( char* cpspath, uint32_t cpspathlen ) {
      char tmp[4098];
      auto len = ::load_str( current_code(), current_code(), N(storage), cpspath, cpspathlen, tmp, 4098 );
      TOKEN_NAME::link link_to_remove;
      uint32_t ipfspathlen;
      len = read_link_from_buffer( tmp, len, link_to_remove, cpspathlen, ipfspathlen );
      cpsio::require_auth( link_to_remove.owner );
      uint32_t stake = link_to_remove.stake;
      ::remove_str( current_code(), N(storage), link_to_remove.cpspath, cpspathlen );
      // Reduce Quota usage in account table
      // How does producer know to free cached file?
   }
   
   void apply_storage_createstore( char* cpspath, uint32_t cpspathlen ) {
      char tmp[4098];
      auto len = ::load_str( current_code(), current_code(), N(storage), cpspath, cpspathlen, tmp, 4098 );
      TOKEN_NAME::link link_to_create;
      uint32_t ipfspathlen;
      len = read_link_from_buffer( tmp, len, link_to_create, cpspathlen, ipfspathlen );
      
      // cpsio::require_auth( producer )
      // How do we validate the require_auth() is a producer?
      // logic goes here to reduce number of tokens and increase quote used using bancor algorithm
      link_to_create.accept = 1;
      ::store_str( current_code(), N(storage), link_to_create.cpspath, cpspathlen, (char*)&link_to_create, len );
   }
   
   void apply_storage_rejectstore( char* cpspath, uint32_t cpspathlen ) {
      char tmp[4098];
      auto len = ::load_str( current_code(), current_code(), N(storage), cpspath, cpspathlen, tmp, 4098 );
      TOKEN_NAME::link link_to_reject;
      uint32_t ipfspathlen;
      len = read_link_from_buffer( tmp, len, link_to_reject, cpspathlen, ipfspathlen );
      // cpsio::require_auth( producer )
      // How do we validate the require_auth() is a producer?
      link_to_reject.accept = 0;
      ::store_str( current_code(), N(storage), link_to_reject.cpspath, cpspathlen, (char*)&link_to_reject, len );
   }
}  // namespace TOKEN_NAME

using namespace TOKEN_NAME;

extern "C" {
    void init()  {
       account owned_account;
       //Initialize storage account only if it does not exist
       if ( !accounts::get( owned_account, N(storage) )) {
          // How do we initialize the storage capacity? By how much here?
          accounts::store( account( storage_tokens(1000ll*1000ll*1000ll) ), N(storage) );
       }
    }

    /// The apply method implements the dispatch of events to this contract
    void apply( uint64_t code, uint64_t action ) {
       if( code == N(storage) ) {
          if( action == N(transfer) ) {
               TOKEN_NAME::apply_storage_transfer( cpsio::current_message< TOKEN_NAME::transfer >() );
          } else if (action == N(setlink) ) {
               TOKEN_NAME::apply_storage_setlink(); 
          } else if (action == N(removelink) ) {
               char tmp[1025];
               auto len = read_message( tmp, 1025 );
               TOKEN_NAME::apply_storage_removelink( tmp, len );
          } else if (action == N(acceptstore) ) {
               char tmp[1025];
               auto len = read_message( tmp, 1025 );
               TOKEN_NAME::apply_storage_createstore( tmp, len );
          } else if (action == N(rejectstore) ) {
               char tmp[1025];
               auto len = read_message( tmp, 1025 );
               TOKEN_NAME::apply_storage_rejectstore( tmp, len );
          } else {
               assert(0, "unknown message");
          }
       } else {
           assert(0, "unknown code");
       }
    }
}
