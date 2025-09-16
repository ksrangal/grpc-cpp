#include "ChatterBox.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <vector>

using chatterbox::ChatterBox;
using chatterbox::Message; 
using grpc::Status;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::ServerReader;
using grpc::ServerReaderWriter;

// This of messages sever send to the client randomly. 
static const std::vector<std::string> messages = 
{
  "I'm not lazy, I'm just running in low-power mode ⚡.",
    "Segmentation fault: Just kidding… or am I? 🤔",
    "Server busy… nah, just scrolling memes.",
    "I would tell you a UDP joke, but you might not get it.",
    "404: Joke not found 😅.",
    "Hold on, installing more RAM…",
    "I don’t have bugs, only undocumented features 🐞.",
    "Keep calm and clear your cache.",
    "I tried to join a gym, but I couldn’t find the right classpath.",
    "Current status: Waiting for coffee ☕.",
    "Request denied: You look suspicious 😎.",
    "Oops… I accidentally formatted C: drive!",
    "Life’s a loop until you break it.",
    "I speak fluent JSON, do you?",
    "Too many requests! Just kidding, I’m lonely 😢."
};

class ChatterBoxImpl final : public ChatterBox::Service
{
    // introvert
    Status chat(ServerContext* context, const Message* request, Message* response)
    {
      log(request);   
      response->set_message(getMessage());
      return Status::OK; 
    }

    // Server side streaming  
    Status chatServerStream(ServerContext* context, const Message* request, ServerWriter<Message>* writer)
    {
      log(request);
      for(auto i=0; i<10; i++) 
      {
        Message response; 
        response.set_message(getMessage()); 
        writer->Write(response);
      } 
      return Status::OK; 
    }

    // Client side streaming 
    Status chatClientStream(ServerContext* context, ServerReader<Message>* reader, Message* response)
    {
      // Collect all messages from client, and finally return OK. 
      Message clientMsg; 
      while (reader->Read(&clientMsg))
      {
        log(&clientMsg); 
      }
     
      response->set_message("I feel you!!!");
      return Status::OK; 
    }

    // Bidirectional streaming
    Status chatStream(ServerContext* context, ServerReaderWriter<Message, Message>* stream)
    {
      Message req; 
      while (stream->Read(&req)) 
      {
        Message res; 
        log(&req); 
        res.set_message(getMessage());
        stream->Write(res);
      }
      return Status::OK; 
    }

    void log(const Message* msg)
    {
      std::cout<<msg->message()<<std::endl;
    }

    std::string getMessage()
    {
      auto idx = std::rand() % messages.size();
      return messages[idx]; 
    }
}; 

void startService(){
  
  std::string listen_addr = "localhost:27001";
  ChatterBoxImpl mysvc; 
  ServerBuilder builder; 
  builder.AddListeningPort(listen_addr, grpc::InsecureServerCredentials());
  builder.RegisterService(&mysvc);

  std::unique_ptr<grpc::Server> server = nullptr; 
  server = builder.BuildAndStart();

  std::cout << "MySerice Listening on: "<< listen_addr << std::endl;

  server->Wait();
}

int main(){
  startService(); 
  return 0;
}
