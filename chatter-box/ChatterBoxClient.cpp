#include "ChatterBox.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <thread>
#include <chrono>

using chatterbox::Message; 
using chatterbox::ChatterBox;
using grpc::Status;
using grpc::Channel;
using grpc::ClientContext;

// Default server address.
constexpr const char* DEFAULT_SERVER_ADDR = "localhost:27001";

// Client randomly send these messages to the server.
static const std::vector<std::string> messages = {
    "Hello, Server! Do you dream of electric sheep?",
    "If I send you a bug, will you fix it or keep it as a pet?",
    "Ping! Also, do you like pineapple on pizza?",
    "Requesting server wisdom… preferably before timeout!",
    "Hey server, I think Linux > Windows. Agree?",
    "Am I fast, or are you just slow?",
    "01001000 01100101 01101100 01101100 01101111 (that means hello in robot)",
    "If this was a game, I’d be sending you extra lives!",
    "Dear server, are you also tired of handling client nonsense?"
};

// Ways client/sever talk to each other. 
enum ChatType
{
  Invalid, 
  Poke, 
  Talk, 
  Listen, 
  Chat
};

static const std::unordered_map< ChatType, std::string > chatToString = 
{
  { Poke,   "poke" },
  { Talk,   "talk" },
  { Listen, "listen" },
  { Chat,   "chat" }
};

static const std::unordered_map< std::string, ChatType > stringToChat= 
{
  { "poke",  Poke },
  { "talk",   Talk },
  { "listen", Listen },
  { "chat",   Chat }
};

ChatType toChatType(const std::string& type) 
{
  auto chatType = stringToChat.find( type ) ; 
  if( chatType!= stringToChat.end() ) 
    return chatType->second; 
  else 
    return ChatType::Invalid; 
}

const std::string toString(ChatType chatType) 
{
  auto chatType_str = chatToString.find( chatType ) ; 
  if ( chatType_str != chatToString.end() ) 
    return chatType_str->second; 
  else 
    return ""; 
}

// client <server-addr> [<talk> | <listen> | <chat>]
// <server-addr> - server connect address 
// <talk>        - client stream 
// <listen>      - server stream 
// <chat>        - both stream 
static std::string helpMsg()
{
  std::string helpMsg = "usage: client <server-addr> [<talk> | <listen> | <chat>]"; 
  return helpMsg;
}

class Client
{
  public:
    Client(std::shared_ptr<Channel> channel) : stub_(ChatterBox::NewStub(channel)) {}

    // This method used to send one message at a time. 
    void poke() 
    {
      Message req;
      Message reply;
      req.set_message( getMessage() );
  
      Status status = stub_->chat( &ctx, req, &reply );
      log( status, reply );
    }

    // This method used to start streaming from client side. 
    void talk() 
    {
      Message reply;
      auto clientStream = stub_->chatClientStream( &ctx, &reply );
  
      // Send few messages to Server. 
      for ( auto i=0; i<10; i++ ) 
      {
        Message req;
        req.set_message( getMessage() ); 
        if ( !clientStream->Write(req) )
          break; // stream closed. 
      }
 
      // Client done with the message, now wait for response. 
      clientStream->WritesDone();
      Status status = clientStream->Finish();
      log( status, reply); 
    }

    // Client sends a message and server starts streaming. 
    void listen() 
    {
      Message req;
      req.set_message( getMessage() );

      // Send a message to server, it will start chatting. 
      auto serverStream = stub_->chatServerStream(&ctx, req);
 
      // Process server messages. 
      Message reply;      
      while(serverStream->Read(&reply)) {
        std::cout << "reply from server: " << reply.message() << std::endl;
      }
    }

    void chat()
    {
      // start the stream. 
      auto stream = stub_->chatStream(&ctx);
  
      std::thread clientThread(
        [&] {
          while ( 1 ) 
          {
            Message req;
            req.set_message( getMessage() ) ; 
            if (!stream->Write(req))
              break; // stream closed. 

            // send msg every second. 
            std::this_thread::sleep_for(std::chrono::seconds(1));
          }
        });

      // Continue processing any message from server. 
      Message reply;
      while(stream->Read(&reply)) {
        std::cout << "reply from server: " << reply.message() << std::endl;
      }
      
      Status status = stream->Finish(); 
      log(status, reply);
    }

    void log(Status& status, const Message& msg)
    {
      if (status.ok()) 
        std::cout<< msg.message() << std::endl; 
      else 
        std::cout<< status.error_code() << " " << status.error_message() << std::endl;
    }

    std::string getMessage()
    {
      auto idx = std::rand() % messages.size();
      return messages[idx]; 
    }

  private: 
    ClientContext ctx;
    std::unique_ptr<ChatterBox::Stub> stub_;
};

int main(int argc, char* argv[])
{
  if ( argc > 3 ) 
  {
    std::cerr<<helpMsg()<<std::endl;
    return 1; 
  }

  auto nargs = argc; 
  auto prog = argv[0]; 
  auto server_addr = DEFAULT_SERVER_ADDR;
  auto chat_type = ChatType::Chat; 

  --nargs; 

  // override server addr if supplied.
  if ( nargs )
  {
    --nargs; 
    server_addr = argv[1]; 
  }

  // override chat type if supplied. 
  if ( nargs ) 
  {
    --nargs; 
    chat_type = toChatType(argv[2]);
  }
    
  Client client(grpc::CreateChannel(server_addr, grpc::InsecureChannelCredentials()));

  switch ( chat_type ) 
  {
    case ChatType::Poke:
      client.poke(); 
      break;
    case ChatType::Talk: 
      client.talk(); 
      break;
    case ChatType::Listen: 
      client.listen(); 
      break;
    case ChatType::Chat: 
      client.chat();
      break;
    default: 
    {
      std::cerr << "Invalid chat type: "<< toString( chat_type ) << std::endl;
      return 1; 
    }
  }

  return 0;
}
