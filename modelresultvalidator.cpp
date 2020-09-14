#include <QFile>

#include "modelresultvalidator.h"
#include "modelresultmeta.h"
#include "logger.h"

/**
 * @brief ModelResultValidator::ModelResultValidator
 * @param parent
 */
ModelResultValidator::ModelResultValidator(QObject *parent)
  : QObject(parent)
  , m_meta(new ModelResultMeta(parent))
{
// EMPTY
}

ModelResultValidator::~ModelResultValidator() {
  E_DEBUG(this) << "Destructor";
}

/**
 * @brief ModelResultValidator::validate
 * @param filename
 * @return
 */
bool ModelResultValidator::validate(const QString& filename) {
  bool result = false;

  if(filename.isEmpty()) {
    E_CRITICAL(this) << "empty filename";
    return result;
  }

  QFile file(filename);
  if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return result;
  }

  while(!file.atEnd()) {
    QByteArray line = file.readLine();
    auto token = m_meta->determineToken(line);

    if(token == ModelResultMeta::TokenType::UNKNOWN) {
      E_DEBUG(this) << "End of meta";
      break;
    } else if(token == ModelResultMeta::TokenType::SIGNALS) {
      m_meta->parseData();

      if(validateSignals(file, line)) {
        result = m_meta->addToken(ModelResultMeta::TokenType::SIGNALS, line);
      }
      // Expected end of meta data
      break;
    } else {
      // Any meta data type that is determined successfully
      if(!m_meta->addToken(token, line)) {
        E_DEBUG(this) << "Error while adding normal token";
        break;
      }
    }
  }

  E_DEBUG(this) << m_meta->getData();

  file.close();

  return result;
}

/**
 * @brief ModelResultValidator::validateSignals
 * @param file
 * @param line
 * @return
 */
bool ModelResultValidator::validateSignals(QFile& file, QByteArray& line) {
  for(size_t i = 0; i < m_meta->getData().varCount; i++) {
    if(i >= MAX_COUNT_OF_SIGNALS) {
      E_WARNING(this) << "Limit of signals reached" << MAX_COUNT_OF_SIGNALS;
      break;
    }

    if(!file.atEnd()) {
      QByteArray signal = file.readLine();
      line.append(signal);
    } else {
      E_CRITICAL(this) << "Unexpected file end";
      return false;
    }
  }

  return true;
}
