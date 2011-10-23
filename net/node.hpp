#ifndef _TORNET_NODE_HPP_
#define _TORNET_NODE_HPP_
#include <tornet/net/channel.hpp>
#include <boost/cmt/thread.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/asio.hpp>

namespace tornet {
  namespace detail { class node_private; }

  class node {
    public:
      typedef boost::shared_ptr<node>               ptr;
      typedef scrypt::sha1                          id_type;
      typedef boost::asio::ip::udp::endpoint        endpoint;
      typedef boost::function<void(const channel&)> new_channel_handler;

      node();
      ~node();

      void init( const boost::filesystem::path& ddir, uint16_t port );

      /**
       *  Connect to the endpoint and return the ID of the node or throw on error.
       */
      id_type connect_to( const endpoint& ep );

      /**
       *  This method will attempt to connect to node_id and then create a new channel to node_port with
       *  the coresponding local_port for return messages.  
       *
       *  @param node_id     - the ID of the node that we wish to connect to.
       *  @param remote_port - the port the remote host is listening for incoming connections.  @ref listen
       *
       *  Throws if unable to resolve or connect to node_id
       */
      channel open_channel( const id_type& node_id, uint16_t remote_chan_num );

      /**
       *  Every time a new channel is created to this node_chan_num, @param on_new_channel is called.
       *  @param service_chan_num - the port accepting new channels
       *  @param service_name - the name of the service running on port (http,chunk,chat,dns,etc)
       *  @param on_new_channel - called from the node's thread when a new channel is created on service_chan_num
       *
       *  @return true if node_chan_num is unused, otherwise false;
       */
      void start_service( uint16_t service_chan_num, const std::string& service_name, 
                          const new_channel_handler& on_new_channel );

      /**
       *  Stop accepting new channels on service_port, does not close
       *  any open channels.
       */
      void close_service( uint16_t service_channel_num );

    private:
      detail::node_private* my; 
  };

};

#endif