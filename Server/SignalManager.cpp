#include "SignalManager.h"

void SignalManager::getWords(const std::string& str, std::set<std::string>& vec){
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

SERVER_COMMAND SignalManager::getServerCommand(const std::string& str){

   for(size_t i = 0; i < serverCommandsList.size(); i++){
      if(str.compare(serverCommandsList.at(i)) == 0){
         return (SERVER_COMMAND)i;
      }
   }
   return SERVER_COMMAND::BAD_COMMAND;
}

CUSTOMER_COMMAND SignalManager::getCustomerCommand(const std::string& str){

   for(size_t i = 0; i < customerCommandsList.size(); i++){
      if(str.compare(customerCommandsList.at(i)) == 0){
         return (CUSTOMER_COMMAND)i;
      }
   }
   return CUSTOMER_COMMAND::BAD_COMMAND;
}

std::string cutMessage(std::string& str){
    size_t start = str.find("/*");
    size_t end = str.find("*/");
    if(start == std::string::npos ||
        end == std::string::npos){
        str.clear();
        return "";
    }
    
    std::string str2 = str.substr(start+2, end-start+1);
    str.erase(start+2, end-start+1);
    return str2;
}

void SignalManager::getServerSignal(const std::string& str2, Signal<SERVER_COMMAND>& signal){
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
      signal.command = SERVER_COMMAND::NO_COMMAND;
      return;
   }

   if((end = str.find_first_of(' ', start+1)) == std::string::npos)
      end = str.size();
   
   sign = str.substr(start, end - start);
   // Якщо незнайдений сигнал - повертаємо структуру з NO_COMMAND
   if((signal.command = getServerCommand(sign)) == SERVER_COMMAND::BAD_COMMAND)
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

void SignalManager::getCustomerSignal(const std::string& str2, Signal<CUSTOMER_COMMAND>& signal){
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
      signal.command = CUSTOMER_COMMAND::NO_COMMAND;
      return;
   }
      
   if((end = str.find_first_of(' ', start+1)) == std::string::npos)
      end = str.size();

   sign = str.substr(start, end - start);
   // Якщо незнайдений сигнал - повертаємо структуру з NO_COMMAND
   if((signal.command = getCustomerCommand(sign)) == CUSTOMER_COMMAND::BAD_COMMAND)
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


// Функції для маніпулювання string-ом
void SignalManager::removeExtraSpaces(std::string& str, size_t start, size_t end){
   if(end == 0)
      end = str.size();
   if(start < 0 || start > str.size() || end < 0 || end > str.size())
      return;
   if(start >= end)
      return;
   // Видаляємо ' ' до першого слова
   for(size_t i = start; i < end; i++){
      if(str[i] != ' ')
         break;
      str.erase(i,1);
      i--;
      end--;
   }
   // Видаляємо ' ' з кінця
   for(size_t i = (str.size()-1); i >= 0; i--){
      if(str[i] != ' ')
        break;
      str.erase(i,1);
      end--;
   }   
   // Видаляємо ' '
   for(size_t i = start; i < end; i++){
      if(str[i] == ' ' && str[i-1] == ' '){
         str.erase(i,1);
         i--;
         end--;
      }
   }
}

void SignalManager::saveWordsInVector(const std::string& str, std::vector<std::string>& out_vec){
   if(str.size() == 0)
      return;
   size_t start = 0, end;
   for (size_t i = 0; i < str.size(); ++i)
   {
      if(str[i] == ' ' || i == (str.size()-1)){
            end = (str[i] == ' ') ? i : i + 1;
            out_vec.push_back(str.substr(start, end - start));
            start = i+1;
      }
   }
}

std::string SignalManager::getWordsToNumber(const std::string& str, const int n){
   size_t start = 0;
   size_t num = 1;
   size_t end = 0;

   for(size_t i = 0; i < str.size(); i++){
      if(str[i] == ' ' || i == (str.size()-1)){
         end = (str[i] == ' ') ? i : i + 1;

         if(num == (size_t)n){
            return str.substr(start, end-start);
         }
         start = i+1;
         num++;
      }
   }
   return "";
}
