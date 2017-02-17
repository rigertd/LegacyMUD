/*!
  \file     VTPTSentence.hpp
  \author   David Rigert
  \created  02/12/2017
  \modified 02/16/2017
  \course   CS467, Winter 2017
 
  \details  This file contains the declarations for the VTPTSentence class.
            The VTPTSentence class represents a Sentence with a verb,
            a text-only direct object, a preposition, and a text-only indirect object.
*/
#ifndef LEGACYMUD_PARSER_VTPTSENTENCE_HPP
#define LEGACYMUD_PARSER_VTPTSENTENCE_HPP

#include "Sentence.hpp"
#include "PartOfSpeech.hpp"
#include "PrepositionType.hpp"

#include <InteractiveNoun.hpp>

#include <vector>

namespace legacymud { namespace parser {

/*!
  \brief Represents a Sentence with a verb, a text-only direct object, a preposition, and a text-only indirect object.

  \warning Be careful when using this sentence type.
           The parser starts from the second-to-last word and moves forward until
           it finds a matching preposition. If it does not find one, it tries
           two words at a time, then three, and so on. The first one or more words
           that match a registered preposition are considered to be the preposition.
           This may result in the direct and indirect object text being parsed
           incorrectly if that text also includes the preposition text.
*/
class VTPTSentence : public virtual Sentence {
    friend class Sentence;
public:
    /*!
      \brief Gets a ParseResult object based on the parsed input string.

      \param[in] tokens     Specifies the token sequence to parse.
      \param[in] playerLex  Specifies the lexical data for objects in the player inventory.
      \param[in] areaLex    Specifies the lexical data for objects in the current area.
      \param[in] grammar    Specifies the grammar rules for the verb being processed.
    */
    virtual ParseResult getResult(const std::vector<Token> &tokens,
                                  const LexicalData &playerLex,
                                  const LexicalData &areaLex,
                                  const Grammar &grammar);

protected:
    /*!
      \brief Default constructor.

      The default constructor initializes the VTPTSentence to an unparsed state.
      This constructor cannot be called directly. 
      Use the Sentence::makeSentence() function to get an instance of 
      the appropriate derived class.
    */
    VTPTSentence(PartOfSpeech verb, VerbType type, engine::CommandEnum command);

private:
    engine::CommandEnum _command;
    VerbType _type;
    PartOfSpeech _verb;
    PartOfSpeech _direct;
    PartOfSpeech _preposition;
    PrepositionType _prepType;
    PartOfSpeech _indirect;
    ParseStatus _status;
};

} }
#endif
