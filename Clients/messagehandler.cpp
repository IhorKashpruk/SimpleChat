#include "messagehandler.h"

MessageHandler::MessageHandler(TCPSocket* sock) : QObject(0), socket(sock)
{

}


bool MessageHandler::waitSignal(const int timeval){
    _timeVal.tv_sec = timeval;
    _timeVal.tv_usec = 0;

    FD_ZERO(&readSet);

    FD_SET(*socket, &readSet);
    int maxDescriptor = *socket;

    int result;
    if(timeval == 0){
        result = select(maxDescriptor+1, &readSet, NULL, NULL, NULL);
    }
    else
        result = select(maxDescriptor+1, &readSet, NULL, NULL, &_timeVal);

    switch(result){
        case -1:
            return false;
            break;
        case 0:
            return false;
            break;
    }
    return true;

}

void MessageHandler::getCustomerSignal(const std::string& str2, Signal& signal){
   std::string str = str2;
   size_t start, end;
   start = end = std::string::npos;
   std::string sign;

   for(size_t i = 0; i < str.size(); i++)
      if(str[i] != ' '){
         start = i;
         break;
      }

   if(start == std::string::npos){
      signal.command = COMMAND::BAD_COMMAND;
      return;
   }

   if((end = str.find_first_of(' ', start+1)) == std::string::npos)
      end = str.size();

   sign = str.substr(start, end - start);
   // Якщо незнайдений сигнал - повертаємо структуру з NO_COMMAND
   if((signal.command = getCustomerCommand(sign)) == COMMAND::BAD_COMMAND)
      return;

   // Якщо є сигнал
   // Якщо немає слів за першим словом
   if(str.size() <= end)
      return;
   // Шукаємо текст
   size_t startApostrophe, endApostrophe;
   startApostrophe = endApostrophe = std::string::npos;
   if((startApostrophe = str.find_first_of('\'')) != std::string::npos)
      if((endApostrophe = str.find_last_of('\'')) != std::string::npos){
         if(startApostrophe != endApostrophe){
            signal.text = str.substr(startApostrophe+1, (endApostrophe-startApostrophe-1));
         }
         else{
            // Помилка тільки одна '
            return;
         }
      }

   // Видаляємо перше слова і текст з str
   str.erase(0, end);
   if(startApostrophe != std::string::npos){
      str.erase(startApostrophe-end, std::string::npos);
   }
   for(char& s: str)
      if(s == ',')
         s = ' ';
   getWords(str, signal.words);

}

COMMAND MessageHandler::getCustomerCommand(const std::string& str){

   for(size_t i = 0; i < customerCommandsList.size(); i++){
      if(str.compare(customerCommandsList.at(i)) == 0){
         return (COMMAND)i;
      }
   }
   return COMMAND::BAD_COMMAND;
}

void MessageHandler::getWords(const std::string& str, std::set<std::string>& vec){
   size_t start, end;
   start = std::string::npos;
   end = std::string::npos;

   for(size_t i = 0; i < str.size(); i++){
      if(start == std::string::npos){
         if(str[i] != ' ')
            start = i;
      }
      else
         if(str[i] == ' ' || i == (str.size()-1))
            end = (str[i] == ' ') ? i : i + 1;
      if(start != std::string::npos && end != std::string::npos){
         vec.insert(str.substr(start, end-start));
         start = end = std::string::npos;
      }
   }
   if(start != std::string::npos){
      vec.insert(str.substr(start, str.size() - start));
   }
}

bool MessageHandler::isBufferEmpty(){
    if(!buffer.size())
        return true;
    return false;
}

std::string MessageHandler::cutMessage(std::string& str){
    size_t start = str.find("/*");
    size_t end = str.find("*/");
    if(start == std::string::npos &&
        end == std::string::npos){
        if(!isBufferEmpty()){
            buffer += str;
        }
        str.clear();
        return "n";
    }
    if(start == std::string::npos){
        if(!isBufferEmpty()){
            buffer += str.substr(0, end+2);
            std::string str2 = buffer;
            buffer.clear();
            return str2;
        }
        str.clear();
        return "n";
    }
    if(end == std::string::npos){
        buffer = str;
        str.clear();
        return "n";
    }
    if((end < start) && !isBufferEmpty()){
        buffer += str.substr(0, end+2);
        str.erase(0, end+2);
        std::string str2 = buffer;
        buffer.clear();
        return str2;
    }

    std::string str2 = str.substr(start, end - start+2);
    str.erase(start, end - start+2);
    return str2;
}

// SLOT
void MessageHandler::run(){
    if(!socket)
        return;

    while(waitSignal()){
        if(socket->readAll(lastMessage) < 1){
            emit closeConnection();
            return;
        }
        // If readAll good
        std::string str2;
        while(lastMessage.size() != 0){
            str2 = cutMessage(lastMessage);
            qDebug() << str2.c_str();
            if(str2.compare("n") != 0){
                str2.erase(str2.find("/*"), 2);
                str2.erase(str2.find("*/"), 2);
                Signal signal;
                getCustomerSignal(str2, signal);
                emit sendSignal(signal);
            }
        }
    }
}
