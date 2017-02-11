/*********************************************************************//**
 * \author      Rachel Weissman-Hohler
 * \created     02/01/2017
 * \modified    02/10/2017
 * \course      CS467, Winter 2017
 * \file        GameLogic.hpp
 *
 * \details     Header file for GameLogic class. Defines the members and 
 *              functions that control the game logic.
 ************************************************************************/

#ifndef GAME_LOGIC_HPP
#define GAME_LOGIC_HPP

#include <string>
#include <queue>
#include "ObjectType.hpp"
#include "CommandEnum.hpp"
#include "ItemPosition.hpp"

namespace legacymud { namespace engine {

class GameObjectManager;
class InteractiveNoun;
class Player;
class Creature;
class NonCombatant;

class GameLogic {
    public:
        GameLogic();
        GameLogic(const GameLogic &otherGameLogic);
        GameLogic & operator=(const GameLogic &otherGameLogic);
        ~GameLogic();

        /*!
         * \brief   Starts a game.
         * 
         * This function starts a new game, if newGame is true, or loads the
         * game from the default file.
         * 
         * \param[in] newGame   Specifies whether or not to start a new game.
         *
         * \return  Returns a bool indicating whether or not starting the game
         *          was successful.
         */
        bool startGame(bool newGame);

        /*!
         * \brief   Starts a game.
         * 
         * This function starts a new game, if newGame is true, or loads the
         * game from the passed-in file.
         * 
         * \param[in] newGame   Specifies whether or not to start a new game.
         * \param[in] fileName  Specifies the file to load the game from, if
         *                      newGame is false.
         *
         * \return  Returns a bool indicating whether or not starting the game
         *          was successful.
         */
        bool startGame(bool newGame, const std::string &fileName);

        /*!
         * \brief   Loads a new player into the game.
         * 
         * This function starts the set-up process for a new player, if they
         * haven't played before, and loads existing players into the game.
         * 
         * \param[in] fileDescriptor    Specifies the player identifier to use when 
         *                              communicating with the server.
         *
         * \return  Returns a bool indicating whether or not loading the player
         *          was successful.
         */
        bool newPlayerHandler(int fileDescriptor);

        /*!
         * \brief   Processes messages from the message queue.
         * 
         * \param[in] numToProcess  Specifies how many messages to process at a
         *                          time.
         *
         * \return  Returns a bool indicating whether or not processing messages
         *          was successful.
         */
        bool processInput(int numToProcess);

        /*!
         * \brief   Adds a new message into the message queue.
         * 
         * This function adds a new message from the server into the message
         * queue for later processing.
         * 
         * \param[in] message           Specifies the message to add.
         * \param[in] fileDescriptor    Specifies the player that sent the message.
         *
         * \return  Returns a bool indicating whether or not adding the message
         *          was successful.
         */
        bool receivedMessageHandler(std::string message, int fileDescriptor);

        /*!
         * \brief   Updates creatures' movements and attacks.
         * 
         * This function updates the position of ambulatory creatures, and the
         * attacks of all creatures.
         *
         * \return  Returns a bool indicating whether or not updating the creatures
         *          was successful.
         */
        bool updateCreatures();

        /*!
         * \brief   Updates players that are in combat.
         * 
         * This function updates all players that are currently in combat, processing
         * actions off of their combat queues or making default attacks.
         *
         * \return  Returns a bool indicating whether or not updating the players
         *          was successful.
         */
        bool updatePlayersInCombat();
    private:

        /*!
         * \brief   Loads the specified player into the game.
         * 
         * This function loads the specified player into the game and stores the
         * fileDescriptor in the player object.
         * 
         * \param[in] aPlayer           Specifies the player to load.
         * \param[in] fileDescriptor    Specifies the player's server identifier.
         *
         * \return  Returns a bool indicating whether or not loading the player
         *          was successful.
         */
        bool loadPlayer(Player *aPlayer, int fileDescriptor);

        /*!
         * \brief   Hibernates the specified player.
         * 
         * This function hibernates the specified player by removing them from the 
         * active game (other players will no longer receive notices about this
         * player). The player must be loaded before they can play more.
         * 
         * \param[in] aPlayer           Specifies the player to hibernate.
         *
         * \return  Returns a bool indicating whether or not hibernating the player
         *          was successful.
         */
        bool hibernatePlayer(Player *aPlayer);

        /*!
         * \brief   Rolls the specifed number of the specified sided dice.
         * 
         * This function "rolls" dice using a random number generator.
         * 
         * \param[in] numSides      Specifies the number of sides each die has.
         * \param[in] numDice       Specifies the number of dice to roll.
         *
         * \return  Returns an int with the results of the roll (the total value of
         *          all the dice rolled).
         */
        int rollDice(int numSides, int numDice);

        /*!
         * \brief   Creates a new instance of the specified object type.
         * 
         * This function asks the specified player for the required fields and creates
         * the specfied new object.
         * 
         * \param[in] aPlayer   Specifies the player who entered the command.
         * \param[in] type      Specifies the type of object to create.
         *
         * \return  Returns a bool indicating whether or not creating the object
         *          was successful.
         */
        bool createObject(Player *aPlayer, ObjectType type);

        /*!
         * \brief   Gets the object type based on the input string.
         * 
         * \param[in] input     Specifies the object type in string format.
         *
         * \return  Returns an ObjectType with the object type.
         */
        ObjectType getObjectType(const std::string &input);

        /*!
         * \brief   Sends the specified message to the specified player.
         * 
         * \param[in] aPlayer   Specifies the player to message.
         * \param[in] message   Specifies the message to send.
         *
         * \return  Returns a bool indicating whether or not sending the message
         *          was successful.
         */
        bool messagePlayer(Player *aPlayer, std::string message);

        /*!
         * \brief   Starts combat between the specfied player and the specified
         *          creature.
         * 
         * \param[in] aPlayer       Specifies the player entering combat.
         * \param[in] aCreature     Specifies the creature entering combat.
         *
         * \return  Returns a bool indicating whether or not entering combat
         *          was successful.
         */
        bool startCombat(Player* aPlayer, Creature *aCreature);

        /*!
         * \brief   Ends combat between the specfied player and the specified
         *          creature.
         * 
         * \param[in] aPlayer       Specifies the player leaving combat.
         * \param[in] aCreature     Specifies the creature leaving combat.
         *
         * \return  Returns a bool indicating whether or not ending combat
         *          was successful.
         */
        bool endCombat(Player *aPlayer, Creature *aCreature);

        /*!
         * \brief   Starts a conversation between the specfied player and the 
         *          specified non-combatant.
         * 
         * \param[in] aPlayer   Specifies the player entering a conversation.
         * \param[in] aNPC      Specifies the non-combatant entering a conversation.
         *
         * \return  Returns a bool indicating whether or not starting a conversation
         *          was successful.
         */
        bool startConversation(Player *aPlayer, NonCombatant *aNPC);

        /*!
         * \brief   Ends the conversation the specfied player is currently engaged in.
         * 
         * \param[in] aPlayer   Specifies the player leaving the conversation.
         *
         * \return  Returns a bool indicating whether or not ending the conversation
         *          was successful.
         */
        bool endConversation(Player *aPlayer);

        /*!
         * \brief   Executes the specified command.
         * 
         * \param[in] aCommand      Specifies the command to execute.
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] directObj     Specifies the direct object received from the 
         *                          parser.
         * \param[in] indirectObj   Specifies the indirect object recevied from the 
         *                          parser.
         * \param[in] stringParam   Specifies the string parameter received from the
         *                          parser.
         * \param[in] aPosition     Specifies the position received from the parser.
         *
         * \return  Returns a bool indicating whether or not executing the command
         *          was successful.
         */
        bool executeCommand(CommandEnum aCommand, Player *aPlayer, InteractiveNoun *directObj, InteractiveNoun *indirectObj, const std::string &stringParam, ItemPosition aPosition = ItemPosition::NONE);

        /*!
         * \brief   Executes the help command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         *
         * \return  Returns a bool indicating whether or not executing the help command
         *          was successful.
         */
        bool helpCommand(Player *aPlayer);

        /*!
         * \brief   Executes the look command.
         * 
         * \param[in] aPlayer   Specifies the player entering the command.
         * \param[in] param     Optionally specifies the object being looked at.
         *
         * \return  Returns a bool indicating whether or not executing the look command
         *          was successful.
         */
        bool lookCommand(Player *aPlayer, InteractiveNoun *param);

        /*!
         * \brief   Executes the listen command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         *
         * \return  Returns a bool indicating whether or not executing the listen command
         *          was successful.
         */
        bool listenCommand(Player *aPlayer);

        /*!
         * \brief   Executes the take command.
         * 
         * \param[in] aPlayer       Specifies the player taking the directObj.
         * \param[in] directObj     Specifies the direct object being taken.
         * \param[in] indirectObj   Optionally specifies the indirect object the directObj is 
         *                          being taken from.
         *                          
         * \return  Returns a bool indicating whether or not executing the take command
         *          was successful.
         */
        bool takeCommand(Player *aPlayer, InteractiveNoun *directObj, InteractiveNoun *indirectObj);

        /*!
         * \brief   Executes the put command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] directObj     Specifies the direct object being placed.
         * \param[in] indirectObj   Specifies the indirect object the directObj is being placed
         *                          on/in/under.
         * \param[in] aPosition     Specifies the position the directObj is being placed in.
         *
         * \return  Returns a bool indicating whether or not executing the put command
         *          was successful.
         */
        bool putCommand(Player *aPlayer, InteractiveNoun *directObj, InteractiveNoun *indirectObj, ItemPosition aPosition);

        /*!
         * \brief   Executes the drop command.
         * 
         * \param[in] aPlayer       Specifies the player dropping the item.
         * \param[in] directObj     Specifies the item being dropped.
         *
         * \return  Returns a bool indicating whether or not executing the drop command
         *          was successful.
         */
        bool dropCommand(Player *aPlayer, InteractiveNoun *directObj);

        /*!
         * \brief   Executes the inventory command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         *
         * \return  Returns a bool indicating whether or not executing the inventory command
         *          was successful.
         */
        bool inventoryCommand(Player *aPlayer);

        /*!
         * \brief   Executes the more command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] directObj     Specifies the direct object received from the 
         *                          parser.
         *                          
         * \return  Returns a bool indicating whether or not executing the more command
         *          was successful.
         */
        bool moreCommand(Player *aPlayer, InteractiveNoun *directObj);

        /*!
         * \brief   Executes the equipment command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         *
         * \return  Returns a bool indicating whether or not executing the equipment command
         *          was successful.
         */
        bool equipmentCommand(Player *aPlayer);

        /*!
         * \brief   Executes the equip command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] directObj     Specifies the direct object received from the 
         *                          parser.
         *
         * \return  Returns a bool indicating whether or not executing the equip command
         *          was successful.
         */
        bool equipCommand(Player *aPlayer, InteractiveNoun *directObj);

        /*!
         * \brief   Executes the unequip command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] directObj     Specifies the direct object received from the 
         *                          parser.
         *
         * \return  Returns a bool indicating whether or not executing the unequip command
         *          was successful.
         */
        bool unequipCommand(Player *aPlayer, InteractiveNoun *directObj);

        /*!
         * \brief   Executes the transfer command.
         * 
         * \param[in] aPlayer       Specifies the player transferring the item.
         * \param[in] directObj     Specifies the direct object being transferred.
         * \param[in] indirectObj   Specifies the indirect object receiving the item.
         *
         * \return  Returns a bool indicating whether or not executing the transfer command
         *          was successful.
         */
        bool transferCommand(Player *aPlayer, InteractiveNoun *directObj, InteractiveNoun *indirectObj);

        /*!
         * \brief   Executes the speak command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] stringParam   Specifies the string parameter received from the
         *                          parser.
         *
         * \return  Returns a bool indicating whether or not executing the speak command
         *          was successful.
         */
        bool speakCommand(Player *aPlayer, const std::string &stringParam);

        /*!
         * \brief   Executes the shout command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] stringParam   Specifies the string parameter received from the
         *                          parser.
         *
         * \return  Returns a bool indicating whether or not executing the shout command
         *          was successful.
         */
        bool shoutCommand(Player *aPlayer, const std::string &stringParam);

        /*!
         * \brief   Executes the whisper command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] indirectObj   Specifies the indirect object recevied from the 
         *                          parser.
         * \param[in] stringParam   Specifies the string parameter received from the
         *                          parser.
         *
         * \return  Returns a bool indicating whether or not executing the whisper command
         *          was successful.
         */
        bool whisperCommand(Player *aPlayer, InteractiveNoun *indirectObj, const std::string &stringParam);

        /*!
         * \brief   Executes the quit command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         *
         * \return  Returns a bool indicating whether or not executing the quit command
         *          was successful.
         */
        bool quitCommand(Player *aPlayer);

        /*!
         * \brief   Executes the go command.
         * 
         * \param[in] aPlayer   Specifies the player that is moving.
         * \param[in] param     Specifies where to go.
         * 
         * \return  Returns a bool indicating whether or not executing the go command
         *          was successful.
         */
        bool goCommand(Player *aPlayer, InteractiveNoun *param);

        /*!
         * \brief   Executes the move command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] directObj     Specifies the direct object received from the 
         *                          parser.
         *                          
         * \return  Returns a bool indicating whether or not executing the move command
         *          was successful.
         */
        bool moveCommand(Player *aPlayer, InteractiveNoun *directObj);

        /*!
         * \brief   Executes the stats command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         *
         * \return  Returns a bool indicating whether or not executing the stats command
         *          was successful.
         */
        bool statsCommand(Player *aPlayer);

        /*!
         * \brief   Executes the quests command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         *
         * \return  Returns a bool indicating whether or not executing the quests command
         *          was successful.
         */
        bool questsCommand(Player *aPlayer);

        /*!
         * \brief   Executes the skills command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         *
         * \return  Returns a bool indicating whether or not executing the skills command
         *          was successful.
         */
        bool skillsCommand(Player *aPlayer);

        /*!
         * \brief   Executes the attack command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] directObj     Specifies the direct object received from the 
         *                          parser.
         * \param[in] indirectObj   Specifies the indirect object recevied from the 
         *                          parser.
         *
         * \return  Returns a bool indicating whether or not executing the attack command
         *          was successful.
         */
        bool attackCommand(Player *aPlayer, InteractiveNoun *directObj, InteractiveNoun *indirectObj);

        /*!
         * \brief   Executes the talk command.
         * 
         * \param[in] aPlayer   Specifies the player entering the command.
         * \param[in] param     Specifies the character being talked to.
         *
         * \return  Returns a bool indicating whether or not executing the talk command
         *          was successful.
         */
        bool talkCommand(Player *aPlayer, InteractiveNoun *param);

        /*!
         * \brief   Executes the shop command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         *
         * \return  Returns a bool indicating whether or not executing the shop command
         *          was successful.
         */
        bool shopCommand(Player *aPlayer);

        /*!
         * \brief   Executes the buy command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] directObj     Specifies the direct object received from the 
         *                          parser.
         *
         * \return  Returns a bool indicating whether or not executing the buy command
         *          was successful.
         */
        bool buyCommand(Player *aPlayer, InteractiveNoun *directObj);

        /*!
         * \brief   Executes the sell command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] directObj     Specifies the direct object received from the 
         *                          parser.
         *
         * \return  Returns a bool indicating whether or not executing the sell command
         *          was successful.
         */
        bool sellCommand(Player *aPlayer, InteractiveNoun *directObj);

        /*!
         * \brief   Executes the search command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] directObj     Specifies the direct object received from the 
         *                          parser.
         *
         * \return  Returns a bool indicating whether or not executing the search command
         *          was successful.
         */
        bool searchCommand(Player *aPlayer, InteractiveNoun *directObj);

        /*!
         * \brief   Executes the useSkill command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] directObj     Specifies the direct object received from the 
         *                          parser.
         * \param[in] indirectObj   Specifies the indirect object recevied from the 
         *                          parser.
         *
         * \return  Returns a bool indicating whether or not executing the useSkillcommand
         *          was successful.
         */
        bool useSkillCommand(Player *aPlayer, InteractiveNoun *directObj, InteractiveNoun *indirectObj);

        /*!
         * \brief   Executes the read command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] directObj     Specifies the direct object received from the 
         *                          parser.
         *
         * \return  Returns a bool indicating whether or not executing the read command
         *          was successful.
         */
        bool readCommand(Player *aPlayer, InteractiveNoun *directObj);

        /*!
         * \brief   Executes the break command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] directObj     Specifies the direct object received from the 
         *                          parser.
         *
         * \return  Returns a bool indicating whether or not executing the break command
         *          was successful.
         */
        bool breakCommand(Player *aPlayer, InteractiveNoun *directObj);

        /*!
         * \brief   Executes the climb command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] directObj     Specifies the direct object received from the 
         *                          parser.
         *
         * \return  Returns a bool indicating whether or not executing the climb command
         *          was successful.
         */
        bool climbCommand(Player *aPlayer, InteractiveNoun *directObj);

        /*!
         * \brief   Executes the turn command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] directObj     Specifies the direct object received from the 
         *                          parser.
         *
         * \return  Returns a bool indicating whether or not executing the turn command
         *          was successful.
         */
        bool turnCommand(Player *aPlayer, InteractiveNoun *directObj);

        /*!
         * \brief   Executes the push command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] directObj     Specifies the direct object received from the 
         *                          parser.
         *
         * \return  Returns a bool indicating whether or not executing the push command
         *          was successful.
         */
        bool pushCommand(Player *aPlayer, InteractiveNoun *directObj);

        /*!
         * \brief   Executes the pull command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] directObj     Specifies the direct object received from the 
         *                          parser.
         *
         * \return  Returns a bool indicating whether or not executing the pull command
         *          was successful.
         */
        bool pullCommand(Player *aPlayer, InteractiveNoun *directObj);

        /*!
         * \brief   Executes the eat command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] directObj     Specifies the direct object received from the 
         *                          parser.
         *
         * \return  Returns a bool indicating whether or not executing the eat command
         *          was successful.
         */
        bool eatCommand(Player *aPlayer, InteractiveNoun *directObj);

        /*!
         * \brief   Executes the drink command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] directObj     Specifies the direct object received from the 
         *                          parser.
         *
         * \return  Returns a bool indicating whether or not executing the drink command
         *          was successful.
         */
        bool drinkCommand(Player *aPlayer, InteractiveNoun *directObj);

        /*!
         * \brief   Executes the edit mode command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         *
         * \return  Returns a bool indicating whether or not executing the edit mode command
         *          was successful.
         */
        bool editModeCommand(Player *aPlayer);

        /*!
         * \brief   Executes the warp command.
         * 
         * \param[in] aPlayer   Specifies the player entering the command.
         * \param[in] param     Specifies where to warp to.
         *
         * \return  Returns a bool indicating whether or not executing the warp command
         *          was successful.
         */ 
        bool warpCommand(Player *aPlayer, InteractiveNoun *param);

        /*!
         * \brief   Executes the copy command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] directObj     Specifies the direct object received from the 
         *                          parser.
         *
         * \return  Returns a bool indicating whether or not executing the copy command
         *          was successful.
         */
        bool copyCommand(Player *aPlayer, InteractiveNoun *directObj);

        /*!
         * \brief   Executes the create command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] indirectObj   Specifies the indirect object recevied from the 
         *                          parser.
         *
         * \return  Returns a bool indicating whether or not executing the create command
         *          was successful.
         */
        bool createCommand(Player *aPlayer, const std::string &stringParam);

        /*!
         * \brief   Executes the edit attribute command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] directObj     Specifies the direct object received from the 
         *                          parser.
         * \param[in] stringParam   Specifies the string parameter received from the
         *                          parser.
         *
         * \return  Returns a bool indicating whether or not executing the edit attribute command
         *          was successful.
         */
        bool editAttributeCommand(Player *aPlayer, InteractiveNoun *directObj, const std::string &stringParam);

        /*!
         * \brief   Executes the edit wizard command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] directObj     Specifies the direct object received from the 
         *                          parser.
         *
         * \return  Returns a bool indicating whether or not executing the edit wizard command
         *          was successful.
         */
        bool editWizardCommand(Player *aPlayer, InteractiveNoun *directObj);

        /*!
         * \brief   Executes the save command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] stringParam   Specifies the string parameter received from the
         *                          parser.
         *
         * \return  Returns a bool indicating whether or not executing the save command
         *          was successful.
         */
        bool saveCommand(Player *aPlayer, const std::string &stringParam);

        /*!
         * \brief   Executes the load command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] stringParam   Specifies the string parameter received from the
         *                          parser.
         *
         * \return  Returns a bool indicating whether or not executing the load command
         *          was successful.
         */
        bool loadCommand(Player *aPlayer, const std::string &stringParam);

        /*!
         * \brief   Executes the delete command.
         * 
         * \param[in] aPlayer       Specifies the player entering the command.
         * \param[in] directObj     Specifies the direct object received from the 
         *                          parser.
         *
         * \return  Returns a bool indicating whether or not executing the delete command
         *          was successful.
         */
        bool deleteCommand(Player *aPlayer, InteractiveNoun *directObj);
        GameObjectManager *manager;
        std::queue<std::tuple<std::string, int>> messageQueue;
};

}}

#endif