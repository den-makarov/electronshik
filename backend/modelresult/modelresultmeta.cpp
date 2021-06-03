#include <sstream>
#include <iomanip>

#include "modelresultmeta.h"
#include "backend/logger/logger.h"

enum class DateFormat {
  RFC2822Date
};

std::time_t fromString(const std::string& str, DateFormat format) {
  std::time_t time = 0;
  switch(format) {
    case DateFormat::RFC2822Date: {
      std::istringstream date(str);
      std::tm tm{};
      date >> std::get_time(&tm, "%a %b %d %H:%M:%S %Y");
      time = std::mktime(&tm);
    }
    break;
  }

  return time;
}

std::string trimString(const std::string& str) {
  std::string result;
  size_t pos = 0;
  while(pos < str.size()) {
    if(str[pos] == ' ' || str[pos] == '\t' || str[pos] == '\n') {
      pos++;
    } else {
      break;
    }
  }
  result = str.substr(pos);
  while(!result.empty()) {
    if(result.back() == ' ' || result.back() == '\t' || result.back() == '\n') {
      result.pop_back();
    } else {
      break;
    }
  }

  return result;
}

std::vector<std::string> splitString(const std::string& str, char delim) {
  std::vector<std::string> result;
  size_t begin = 0;
  size_t end = str.find(delim);
  while(end != std::string::npos) {
    result.push_back(str.substr(begin, end - begin));
    begin = end + 1;
    end = str.find(delim, begin);
  }
  if(begin != 0) {
    result.push_back(str.substr(begin, str.size() - begin));
  }

  return result;
}

// Map for search token
const std::unordered_map<std::string, ModelResultMeta::TokenType> ModelResultMeta::STR_TO_TOKEN_TYPE = {
  {"Title", TokenType::TITLE},
  {"Date", TokenType::DATE},
  {"Plotname", TokenType::PLOTNAME},
  {"Flags", TokenType::FLAGS},
  {"No. Variables", TokenType::VAR_COUNT},
  {"No. Points", TokenType::POINT_COUNT},
  {"Variables", TokenType::SIGNALS}
};

/**
 * @brief ModelResultMeta::determineToken
 * @param str
 * @return
 */
ModelResultMeta::TokenType ModelResultMeta::determineToken(const std::string& str) const {
  TokenType token = TokenType::UNKNOWN;

  auto list = splitString(str, ':');
  if(!list.empty()) {
    auto type = STR_TO_TOKEN_TYPE.find(list[0]);
    if(type != STR_TO_TOKEN_TYPE.end()) {
     token = type->second;
   }
  }

  return token;
}

/**
 * @brief ModelResultMeta::addToken
 * @param type
 * @param str
 * @return
 */
bool ModelResultMeta::addToken(ModelResultMeta::TokenType type, const std::string& str) {
  bool result = false;

  if(type == TokenType::UNKNOWN) {
    Logger::log(Model::ModelMessage::ERROR_ADD_UNKNOWN_TOKEN, str);
  } else {
    auto pos = str.find(':');
    if(pos != std::string::npos) {
      m_tokens.push_back({trimString(str.substr(pos + 1)), type});
      result = true;
    }
  }

  return result;
}

/**
 * @brief ModelResultMeta::addSignalToken
 * @param list
 * @return
 */
void ModelResultMeta::parseSignalToken(const std::string& str) {
  auto list = splitString(str, '\t');
  // @NOTE: If there are any signals, size of container should be definitely more than 2
  // and number of items should be ratio of 3
  if(list.size() > 2 && !(list.size() % 3)) {
    auto str_ref = trimString(list[0]);
    str_ref.resize(str_ref.size() - 1);

    // @NOTE: each signal consists of number, signal name and signal units
    // for example: 0 Idd Amps
    for(size_t i = 0; i < list.size();) {
      SignalDescriptor signal;
      signal.name = trimString(list[++i]);
      signal.units = trimString(list[++i]);
      m_data.signalSet.push_back(signal);
      i++;
    }
  }
}

/**
 * @brief ModelResultMeta::peekTokenData
 * @param token
 * @return
 */
std::string_view ModelResultMeta::peekTokenData(TokenType token) const {
  for(const auto & item : m_tokens) {
    if(item.type == token) {
      return std::string_view(item.data.c_str(), item.data.size());
    }
  }

  return {};
}

/**
 * @brief ModelResultMeta::parseToken
 * @param token
 * @param data
 */
void ModelResultMeta::parseToken(TokenType token, const std::string& data) {
  switch(token) {
  case TokenType::TITLE:
    m_data.title = data;
    break;
  case TokenType::DATE:
    m_data.timeStamp = fromString(data, DateFormat::RFC2822Date);
    break;
  case TokenType::PLOTNAME:
    m_data.plotname = data;
    break;
  case TokenType::FLAGS:
    m_data.flags = Flags::REAL;
    break;
  case TokenType::VAR_COUNT:
    m_data.varCount = std::stoul(data);
    break;
  case TokenType::POINT_COUNT:
    m_data.points = std::stoul(data);
    break;
  case TokenType::SIGNALS:
    parseSignalToken(data);
    break;
  case TokenType::UNKNOWN:
    break;
  }
}

/**
 * @brief ModelResultMeta::parseData
 */
void ModelResultMeta::parseData() {
  for(const auto& item : m_tokens) {
    parseToken(item.type, item.data);
  }
}

ModelResultMeta::SignalDescriptor ModelResultMeta::Token::splitToken(char delim) const {
  if(type == TokenType::SIGNALS) {
    auto list = splitString(trimString(data), delim);
    return {list.front(), list.back()};
  } else {
    return {"EMPTY", "UNIT"};
  }
}

/**
 * @brief operator <<
 * @param log
 * @param data
 * @return
 */
std::ostream& operator<<(std::ostream& out, const ModelResultMeta::Flags& data) {
  switch(data) {
    case ModelResultMeta::Flags::REAL: out << "REAL"; break;
    case ModelResultMeta::Flags::UNKNOWN: out << "UNKNOWN"; break;
  }
  return out;
}

std::ostream& operator<<(std::ostream& out, const ModelResultMeta::SignalDescriptor& signal) {
  out << "Signal " << signal.name << " [" << signal.units << "]";
  return out;
}

std::ostream& operator<<(std::ostream& out, ModelResultMeta::TokenType token) {
  std::string str;
  switch (token) {
  case ModelResultMeta::TokenType::DATE: str = "Date"; break;
  case ModelResultMeta::TokenType::TITLE: str = "Title"; break;
  case ModelResultMeta::TokenType::PLOTNAME: str = "Plotname"; break;
  case ModelResultMeta::TokenType::FLAGS: str = "Flags"; break;
  case ModelResultMeta::TokenType::VAR_COUNT: str = "Number of variables"; break;
  case ModelResultMeta::TokenType::POINT_COUNT: str = "Number of points"; break;
  case ModelResultMeta::TokenType::SIGNALS: str = "Variables"; break;
  case ModelResultMeta::TokenType::UNKNOWN: str = "Unknown"; break;
  }

  out << str;
  return out;
}

/**
 * @brief operator <<
 * @param log
 * @param data
 * @return
 */
std::ostream& operator<<(std::ostream& out, const ModelResultMeta::Data& data) {
  out << "Model result meta data:\n";
  out << "Title: " << data.title << "\n";
  out << "Plotname: " << data.plotname << "\n";
  out << "Date: " << std::ctime(&data.timeStamp);
  out << "Flags: " << data.flags << "\n";
  out << "Number of signals: " << data.varCount << /*data.signalSet <<*/ "\n";
  out << "Number of points: " << data.points;
  return out;
}