/*!
  \file     VDISentence.cpp
  \author   David Rigert
  \created  02/12/2017
  \modified 02/13/2017
  \course   CS467, Winter 2017
 
  \details  This file contains the implementation of the VDISentence class.
*/
#include "VDISentence.hpp"

namespace legacymud { namespace parser {

VDISentence::VDISentence(PartOfSpeech verb, VerbType type, engine::CommandEnum command) {
    _command = command;
    _verb = verb;
    _type = type;
    _status = ParseStatus::UNPARSED;
}

// Gets a ParseResult object based on the parsed input string.
ParseResult VDISentence::getResult(const std::vector<Token> &tokens, const LexicalData &playerLex, const LexicalData &areaLex) {
    ParseResult result;

    return result;
}

} }