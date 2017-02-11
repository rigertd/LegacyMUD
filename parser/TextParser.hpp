/*!
  \file     TextParser.hpp
  \author   David Rigert
  \created  02/02/2017
  \modified 02/10/2017
  \course   CS467, Winter 2017
 
  \details  This file contains the declarations for the TextParser class and
            its associated enumerated types and structures.
            The TextParser class validates text input from users and
            converts it into an enum that the game engine uses to determine
            which action to perform.
*/
#ifndef LEGACYMUD_PARSER_TEXTPARSER_HPP
#define LEGACYMUD_PARSER_TEXTPARSER_HPP

#include "WordManager.hpp"

#include <CommandEnum.hpp>
#include <ItemPosition.hpp>

#include <vector>
#include <map>
#include <string>
#include <queue>

namespace legacymud { namespace engine {
    class InteractiveNoun;
}}

namespace legacymud { namespace parser {

/*!
  \typedef legacymud::parser::WordMap
  \brief Specifies the type of the lookup table used for mapping aliases to InteractiveNoun pointers.
*/
typedef std::multimap<std::string, engine::InteractiveNoun *> WordMap;

/*!
  \enum legacymud::parser::TextParseStatus
  \brief Enumerates the results of the text parsing operation.

  This enumerated type is returned by the legacymud::parser::TextParser::parse()
  function to communicate the result of the operation to the caller.
*/
enum class TextParseStatus {
    INVALID_VERB = 0,   //!< Input string contains invalid verb.
    UNAVAILABLE_VERB,   //!< Input string contains valid verb but not in current area.
    INVALID_NOUN,       //!< Input string contains valid verb but invalid noun.
    UNAVAILABLE_NOUN,   //!< Input string contains valid noun but not in current area.
    VALID               //!< Input string contains all valid words.
};

/*!
  \enum legacymud::parser::VerbType
  \brief Enumerates the possible types of verb.

  This enumerated type is used to prioritize parse results by verb type.
*/
enum class VerbType {
    INVALID = 0,
    UNAVAILABLE,
    GLOBAL,
    LOCAL,
    BUILDER,
    EDITMODE
};

/*!
  \brief Stores the parsing result of the TextParser.

  This structure stores the parsing result of the TextParser.parse() function.
  The data is only valid if the TextParser.parse() function returns
  \c TextParseStatus::VALID.
*/
struct TextParseResult {
    /*!
      \brief The type of verb.
    */
    VerbType type;

    /*!
      \brief The \c CommandEnum that the verb maps to.
    */
    engine::CommandEnum command;

    /*!
      \brief The object that matches the direct noun of the input text.

      This member only points to an object if the input text has a direct noun
      that matches an object in the current area and a verb that matches
      a verb alias of the object or a global verb. If there is no match, this 
      is set to nullptr.
    */
    engine::InteractiveNoun *direct;

    /*!
      \brief A list of objects that match the indirect noun of the input text.

      This list only contains objects if the input text has an indirect noun.
      that matches one or more objects in the current area and a verb that
      matches a verb alias of the object or a global verb.
      If the sentence does not have an indirect noun, this list is empty.
    */
    std::vector<engine::InteractiveNoun *> indirect;

    /*!
      \brief The position that the direct object should be placed in.

      This specifies the intended position of the direct object.
      If no position was specified, this will be ItemPosition::NONE.
    */
    engine::ItemPosition position;

    /*!
      \brief Stores any unparsed text after the action.

      This value holds all text that could not be parsed after the action.
      When a message command such as SPEAK, WHISPER or SHOUT is used, this
      holds the message.
      When input text cannot be correctly parsed, this holds the part that
      could not be parsed.
    */
    std::string unparsed;

    TextParseResult() : TextParseResult(VerbType::INVALID) {}
    
    TextParseResult(VerbType t) {
        type = t;
        command = engine::CommandEnum::INVALID;
        direct = nullptr;
        position = engine::ItemPosition::NONE;
    }
};

/*!
  \brief Contains a function for parsing text input to determine which command to run.

  This class is used for parsing text commands entered by players. The actual lexical
  data is provided by the game engine or the WordManager class. This class merely encapsulates
  the parsing logic. Each thread should have its own instance of this class.
*/
class TextParser {
public:
    TextParser() {}
    ~TextParser();

    /*!
      \brief Converts text input from players into potential commands to be run.

      This function takes the text entered by a player and analyzes it to determine
      what action the player is attempting to take. If the input string corresponds
      to a valid command, that command is stored in the list specified in the 
      \a candidates parameter.

      The input text is parsed in the following order:
        -# Verb
        -# Direct object
        -# Preposition
        -# Indirect object

      If any of these parts of speech are invalid, the function immediately returns 
      an unsuccessful TextParseStatus value without analyzing the rest of the text.
      The unparsed portion of the input text is placed in the \c unparsed member of
      the TextParseResult object.
      
      \param[in]  input         Specifies the input text to parse.
      \param[in]  playerVerbMap Specifies a lookup table of verb aliases mapped to
                                pointers to InteractiveNoun objects in the player's
                                inventory.
      \param[in]  playerNounMap Specifies a lookup table of noun aliases mapped to
                                pointers to InteractiveNoun objects in the player's
                                inventory.
      \param[in]  areaVerbMap   Specifies a lookup table of verb aliases mapped to
                                pointers to InteractiveNoun objects in the current area.
      \param[in]  areaNounMap   Specifies a lookup table of noun aliases mapped to
                                pointers to InteractiveNoun objects in the current area.
      \param[out] candidates    Holds the valid actions and matching objects found
                                by the parser.
      \param[in]  isAdmin       Specifies whether the player has admin permissions.
                                Defaults to false.
      \param[in]  editMode      Specifies whether the player is in edit mode.
                                This is ignored if \a isAdmin is false. 
                                Defaults to false.

      \pre \a input, \a areaNouns, and \a candidates are valid objects.
      \pre \a candidates is empty.

      \post \a candidates contains a TextParseResult for each potential match.

      \return Returns whether the input text was successfully converted into an action.

      \retval TextParseStatus::VALID            There is at least one successful
                                                TextParseResult in \a candidates.
      \retval TextParseStatus::INVALID_VERB     The input text does not begin with a valid verb.
      \retval TextParseStatus::INVALID_NOUN     The input text begins with a valid verb,
                                                but includes an invalid direct or indirect noun.
                                                This result may also be returned if the text
                                                includes an invalid preposition.
      \retval TextParseStatus::UNAVAILABLE_VERB The input text begins with a valid verb,
                                                but it does not work with any object in
                                                the current area.
      \retval TextParseStatus::UNAVAILABLE_NOUN The input text contains all valid words,
                                                but the direct or indirect noun does not match
                                                any of the objects in the current area.
    */
    TextParseStatus parse(
        const std::string &input, 
        const WordMap &playerVerbMap,
        const WordMap &playerNounMap,
        const WordMap &areaVerbMap,
        const WordMap &areaNounMap,
        std::vector<TextParseResult> &candidates,
        bool isAdmin = false,
        bool editMode = false
                         );
private:
    // Stores a single token
    struct Token {
        std::string original;
        std::string normalized;
    };

    // Stores the start and end indices of a grammar type
    struct Range {
        size_t start;
        size_t end;

        Range() : Range(0, 0) {}
        Range(size_t s, size_t e) {
          start = s;
          end = e;
        }
    };

    // Stores a candidate match
    struct Match {
        std::string verbAlias;
        std::map<std::string, engine::InteractiveNoun *> directObjs;
        PrepositionType preposition;
        std::map<std::string, engine::InteractiveNoun *> indirectObjs;
        std::string unparsed;
        VerbInfo verbInfo;
        TextParseStatus status;
    };

    // Manages the result candidates and keeps track of the best so far.
    class ResultContainer {
    public:
        ResultContainer();
        // Gets the best TextParseStatus so far.
        TextParseStatus getBestStatus() const;
        // Gets the result(s) with the most complete parse status.
        // If there are multiple results with the same TextParseStatus,
        // the ones with the highest priority verb type are returned.
        TextParseStatus getBestResults(std::vector<TextParseResult> &results) const;
        // Adds a result to the ResultContainer and updates the highest status so far.
        void addResult(TextParseStatus status, TextParseResult result);
        size_t getResultCount() const;

    private:
        size_t _count;
        TextParseStatus _bestSoFar;
        std::multimap<TextParseStatus, TextParseResult> _results;
    };

    // Creates a token list from the input string
    std::vector<Token> tokenizeInput(const std::string &);

    // Join the specified range of a token list into a normalized string
    std::string joinNormalizedTokens(const std::vector<Token> &tokens, Range range, bool skipIgnoreWords);

    // Join the specified range of a token list into the original string
    std::string joinOriginalTokens(const std::vector<Token> &tokens, Range range);

    // Find a matching alias in a WordMap
    std::string findLongestLocalAlias(const WordMap &lookupTable, const std::vector<Token> &tokens, Range &range);

    // Find a matching alias using the specified find function
    std::string findLongestGlobalAlias(bool (*hasAlias)(std::string), const std::vector<Token> &tokens, Range &range);

    // Tries to find a global match based on input tokens and maps
    Match parseGlobal(
        const std::vector<Token> &tokens,
        bool (*hasAlias)(std::string),
        VerbInfo (*getVerb)(std::string),
        const WordMap &playerNounMap,
        const WordMap &areaNounMap
    );

    // Tries to find a local match based on input tokens and maps
    Match parseLocal(
        const std::vector<Token> &tokens,
        const WordMap &playerVerbMap,
        const WordMap &playerNounMap,
        const WordMap &areaVerbMap,
        const WordMap &areaNounMap
    );

    // Converts a PrepositionType value to an ItemPosition value
    engine::ItemPosition prepositionToPosition(PrepositionType preposition);
    
    // Generates TextParseResults from the Match and adds them to the container.
    void addMatchToResults(VerbType type, const Match m, ResultContainer &results);
};

}}

#endif