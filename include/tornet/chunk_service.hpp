#ifndef _TN_CHUNK_SERVICE_HPP_
#define _TN_CHUNK_SERVICE_HPP_
#include <fc/shared_ptr.hpp>
#include <fc/fwd.hpp>
#include <fc/vector_fwd.hpp>
#include <tornet/tornet_file.hpp>

namespace fc {
  class path;
  class sha1;
  class ostream;
  class mutable_buffer;
}

namespace tn {

  namespace db {
    class chunk;
    class publish;
  }
  class node;
  class tornet_file;
  class chunk_service;
  class download_status;

  /**
   *  Determins if data is sufficiently random for the purposes of
   *  the chunk service.
   */
  bool      is_random( const fc::vector<char>& data );
  /**
   *  Takes arbitrary data and 'randomizes' it returning the MT19937 key
   *  that results in a random sequence that satisifies is_random()
   *
   *  Modifies data using the random sequence.
   */
  uint64_t  randomize( fc::vector<char>& data );

  /**
   *  Reverses the randomization performed by randomize
   *  @param seed - the value returned by randomize.
   */
  void      derandomize( uint64_t seed, fc::vector<char>& data );
  void      derandomize( uint64_t seed, const fc::mutable_buffer& b );

  /**
   *  Provides an interface to two chunk databases, one local and one cache.
   *
   *  The local chunk database stores the chunks the user imports or downloads without
   *  respect to their distance from the node or access patterns. 
   *
   *  The cache chunk database stores chunks for other people so that it might earn
   *  credit with other nodes.  This cache database stores chunks based upon their
   *  access rate and distance from the node.
   *
   *  All chunks are random data encrypted via blowfish from the original file.  The blowfish
   *  key is the hash of the original file.  To restore a file you must know the hash of the
   *  file description chunk as well as the hash of the resulting file.  
   */
  class chunk_service : virtual public fc::retainable {
    public:
      typedef fc::shared_ptr<chunk_service> ptr;

      chunk_service( const fc::path&      dbdir, const fc::shared_ptr<tn::node>& n );

      virtual ~chunk_service();

      void shutdown();
       
      fc::shared_ptr<tn::db::chunk>&    get_cache_db();
      fc::shared_ptr<tn::db::chunk>&    get_local_db();
      fc::shared_ptr<tn::db::publish>&  get_publish_db();
      fc::shared_ptr<tn::node>&         get_node();

      /**
       *  Loads infile from disk, creates a tornet file and returns the tornet_id and thechecksum.
       *  Optionally writes the tornet file to disk as outfile.
       */
      void import( const fc::path& infile, 
                   fc::sha1& tornet_id, fc::sha1& checksum, uint64_t& seed,
                   const fc::path& outfile  );

      /**
       *  Given a tornet_id and checksum, find the chunk, decrypt the tornetfile then find the
       *  chunks from the tornet file and finally reconstruct the file on disk.
       */
      void export_tornet( const fc::sha1& tornet_id, const fc::sha1& checksum, uint64_t seed );

      /**
       *  Starts a new download operation.
       */
      fc::shared_ptr<download_status> download_tornet( const fc::sha1& tornet_id, const fc::sha1& checksum, uint64_t seed, fc::ostream& out );
     
      /**
       *  Reads the data for the chunk from the cache or local database.
       */
      fc::vector<char> fetch_chunk( const fc::sha1& chunk_id );
      tornet_file      fetch_tornet( const fc::sha1& tornet_id, const fc::sha1& checksum, uint64_t seed );

      void publish_tornet( const fc::sha1& tornet_id, const fc::sha1& checksum, uint64_t seed, uint32_t rep = 3 );

      void enable_publishing( bool state );
      bool publishing_enabled()const;

    private:
      class impl;
      fc::fwd<impl,64> my;
  };

}


#endif // _CHUNK_SERVICE_HPP_
