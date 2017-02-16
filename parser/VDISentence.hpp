/*!
  \file     VDISentence.hpp
  \author   David Rigert
  \created  02/12/2017
  \modified 02/15/2017
  \course   CS467, Winter 2017
 
  \details  This file contains the declarations for the VDISentence class.
            The VDISentence class represents a Sentence with a verb, a direct 
            object, and an indirect object.
*/
#ifndef LEGACYMUD_PARSER_VDISENTENCE_HPP
#define LEGACYMUD_PARSER_VDISENTENCE_HPP

#include "Sentence.hpp"
#include "PartOfSpeech.hpp"

#include <InteractiveNoun.hpp>

#include <vector>

namespace legacymud { namespace parser {

/*!
  \brief Represents a Sentence with a verb, a direct object, and an indirect object.
*/
class VDISentence : public virtual Sentence {
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

      The default constructor initializes the VDISentence to an unparsed state.
      This constructor cannot be called directly. 
      Use the Sentence::makeSentence() function to get an instance of 
      the appropriate derived class.
    */
    VDISentence(PartOfSpeech verb, VerbType type, engine::CommandEnum command);

private:
    engine::CommandEnum _command;
    VerbType _type;
    PartOfSpeech _verb;
    PartOfSpeech _direct;
    std::vector<engine::InteractiveNoun*> _directObjects;
    PartOfSpeech _indirect;
    std::vector<engine::InteractiveNoun*> _indirectObjects;
    ParseStatus _status;
};

} }
#endif
