#include <map>

#include "gui_message.h"

namespace Gui {

std::string getMessagePhrase(GuiMessage msg) {
  static std::map<GuiMessage, std::string> msgPhrases = {
    {GuiMessage::DEBUG_LAST, "Restricted to use"},
    {GuiMessage::INFO_LAST, "Restricted to use"},
    {GuiMessage::WARNING_LAST, "Restricted to use"},
    {GuiMessage::ERROR_LAST, "Restricted to use"},
    // Debug messagess
    {GuiMessage::DEBUG_ATTEMPT_ADD_ITEM_TWICE, "Attempt to add item that is already exists: %1%"},
    // Info messages
    // Warning messages
    // Error messages
    {GuiMessage::ERROR_SELECTION_INVALID, "Valid selection is expected"},
    {GuiMessage::ERROR_NO_META_DATA, "Meta data is expected"},
    {GuiMessage::ERROR_ATTEMPT_PLOT_SAME_SIGNAL, "Attempt to add signal to graph that already has signal with similar name %1%"},
    {GuiMessage::ERROR_NO_DATA_TO_PLOT, "No data to plot graphs"},
    {GuiMessage::ERROR_NO_SELECTED_SIGNALS_TO_PLOT, "No signals were selected to plot"},
  };

  return msgPhrases.at(msg);
}

} // namespace Gui
