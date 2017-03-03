/*********************************************************************//**
 * \author      Rachel Weissman-Hohler
 * \created     02/10/2017
 * \modified    03/02/2017
 * \course      CS467, Winter 2017
 * \file        Player.cpp
 *
 * \details     Implementation file for Player class. 
 ************************************************************************/

#include <iostream>
#include "Player.hpp"
#include "Area.hpp"
#include "Quest.hpp"
#include "QuestStep.hpp"
#include "NonCombatant.hpp"
#include "PlayerClass.hpp"
#include "InteractiveNoun.hpp"
#include "SpecialSkill.hpp"
#include "Item.hpp"
#include "Container.hpp"

namespace legacymud { namespace engine {

const int START_HEALTH = 10;
const int START_SPECIAL_PTS = 10;
const int START_MONEY = 10;
const int MAX_INVENTORY_WEIGHT = 30;

Player::Player()
: Combatant()
, experiencePoints(0) 
, level(1)
, size(CharacterSize::MEDIUM)
, playerClass(nullptr)
, inConversation(nullptr)
, username("")
, active(false)
, fileDescriptor(-1)
, editMode(false) 
{ }


Player::Player(CharacterSize size, PlayerClass *aClass, std::string username, int FD, std::string name, std::string description, Area *startArea)
: Combatant(START_HEALTH, startArea, START_SPECIAL_PTS, name, description, START_MONEY, startArea, MAX_INVENTORY_WEIGHT)
, experiencePoints(0)
, level(1)
, size(size)
, playerClass(aClass)
, inConversation(nullptr)
, username(username)
, active(true)
, fileDescriptor(FD)
, editMode(false)
{
    addAllLexicalData(aClass); 
    addAllLexicalData(aClass->getSpecialSkill());
}


Player::Player(CharacterSize size, PlayerClass *aClass, std::string username, int FD, int maxHealth, Area *spawnLocation, int maxSpecialPts, std::string name, std::string description, int money, Area *aLocation, int maxInventoryWeight)
: Combatant(maxHealth, spawnLocation, maxSpecialPts, name, description, money, aLocation, maxInventoryWeight)
, experiencePoints(0)
, level(1)
, size(size)
, playerClass(aClass)
, inConversation(nullptr)
, username(username)
, active(true)
, fileDescriptor(FD)
, editMode(false)
{ 
    addAllLexicalData(aClass); 
    addAllLexicalData(aClass->getSpecialSkill());
}


Player::Player(CharacterSize size, PlayerClass *aClass, std::string username, int FD, int maxHealth, Area *spawnLocation, int maxSpecialPts, int dexterity, int strength, int intelligence, std::string name, std::string description, int money, Area *aLocation, int maxInventoryWeight, int anID)
: Combatant(maxHealth, spawnLocation, maxSpecialPts, dexterity, strength, intelligence, name, description, money, aLocation, maxInventoryWeight, anID)
, experiencePoints(0)
, level(1)
, size(size)
, playerClass(aClass)
, inConversation(nullptr)
, username(username)
, active(true)
, fileDescriptor(FD)
, editMode(false)
{ 
    addAllLexicalData(aClass); 
    addAllLexicalData(aClass->getSpecialSkill());
}


/*Player::Player(const Player &otherPlayer){

}


Player & Player::operator=(const Player &otherPlayer){

}


Player::~Player(){

}*/


int Player::getExperiencePoints() const{
    return experiencePoints.load();
}


int Player::getLevel() const{
    return level.load();
}


CharacterSize Player::getSize() const{
    return size.load();
}


std::string Player::getUser() const{
    std::lock_guard<std::mutex> usernameLock(usernameMutex);
    return username;
}  


PlayerClass* Player::getPlayerClass() const{
    std::lock_guard<std::mutex> playerClassLock(playerClassMutex);
    return playerClass;
}


NonCombatant* Player::getInConversation() const{
    std::lock_guard<std::mutex> inConversationLock(inConversationMutex);
    return inConversation;
}


bool Player::isActive() const{
    return active.load();
}


int Player::getFileDescriptor() const{
    return fileDescriptor.load();
}


bool Player::queueIsEmpty() const{
    std::lock_guard<std::mutex> combatQueueLock(combatQueueMutex);
    return combatQueue.empty();
}


bool Player::isEditMode() const{
    return editMode.load();
}


std::map<Quest*, std::pair<int, bool>> Player::getQuestList() const{
    std::lock_guard<std::mutex> questListLock(questListMutex);
    return questList;
}


std::pair<int, bool> Player::getQuestCurrStep(Quest *aQuest) const{
    if (aQuest != nullptr){
        std::lock_guard<std::mutex> questListLock(questListMutex);
        int found = questList.count(aQuest);

        if (found == 1){
            return questList.at(aQuest);
        }
    }
    return std::make_pair(-1, false);
}


const parser::LexicalData& Player::getLexicalData() const{
    std::lock_guard<std::mutex> lexicalLock(lexicalMutex);
    return inventoryLexicalData;
}


int Player::addToExperiencePts(int gainedXP){
    int newXP = experiencePoints.load() + gainedXP;
    experiencePoints.store(newXP);

    return experiencePoints.load();
}


bool Player::levelUp(){
    return false;
}


bool Player::setSize(CharacterSize size){
    this->size.store(size);

    return true;
}


bool Player::setPlayerClass(PlayerClass *aClass){
    std::lock_guard<std::mutex> playerClassLock(playerClassMutex);
    removeAllLexicalData(playerClass);
    playerClass = aClass;
    addAllLexicalData(playerClass);

    return true;
}


bool Player::setActive(bool active){
    this->active.store(active);

    return true;
} 


bool Player::setFileDescriptor(int FD){
    fileDescriptor.store(FD);

    return true;
}


bool Player::activate(int FD){
    fileDescriptor.store(FD);
    active.store(true);

    return true;
}


bool Player::setInConversation(NonCombatant *anNPC){
    std::lock_guard<std::mutex> inConversationLock(inConversationMutex);
    inConversation = anNPC;

    return true;
}


Command* Player::getNextCommand(){
    std::lock_guard<std::mutex> combatQueueLock(combatQueueMutex);
    Command *nextCommand = nullptr;
    if (!combatQueue.empty()){
        nextCommand = combatQueue.front();
        combatQueue.pop();
    }

    return nextCommand;
}


bool Player::addCommand(Command *aCommand){
    std::lock_guard<std::mutex> combatQueueLock(combatQueueMutex);
    combatQueue.push(aCommand);

    return true;
}


bool Player::setEditMode(bool editing){
    editMode.store(editing);

    return true;
}


bool Player::addOrUpdateQuest(Quest *aQuest, int step, bool complete){
    int found;

    if (aQuest != nullptr){
        std::lock_guard<std::mutex> questListLock(questListMutex);
        found = questList.count(aQuest);

        if (found != 1){
            // add quest
            questList[aQuest] = std::make_pair(step, complete);
            addAllLexicalData(aQuest);
            addAllLexicalData(aQuest->getStep(step));
        } else {
            // update quest
            removeAllLexicalData(aQuest->getStep(questList.at(aQuest).first));
            questList.at(aQuest) = std::make_pair(step, complete);
            addAllLexicalData(aQuest->getStep(step));
        }
        return true;
    }   
    return false;
}


bool Player::addToInventory(Item *anItem){
    bool success = false; 
    Container *aContainer = nullptr;
    std::vector<Item*> contents;

    if (anItem != nullptr){
        success = Character::addToInventory(anItem);

        if (success){
            addAllLexicalData(anItem);

            if (anItem->getObjectType() == ObjectType::CONTAINER){
                aContainer = dynamic_cast<Container*>(anItem);
                if (aContainer != nullptr){
                    contents = aContainer->getAllContents();
                    for (auto content : contents){
                        addAllLexicalData(content);
                    }
                }
            }
        }
    }
    return success;
}


bool Player::removeFromInventory(Item *anItem){
    bool success = false;  
    Container *aContainer = nullptr;
    std::vector<Item*> contents;

    if (anItem != nullptr){
        success = Character::removeFromInventory(anItem);

        if (success){
            removeAllLexicalData(anItem);

            if (anItem->getObjectType() == ObjectType::CONTAINER){
                aContainer = dynamic_cast<Container*>(anItem);
                if (aContainer != nullptr){
                    contents = aContainer->getAllContents();
                    for (auto content : contents){
                        removeAllLexicalData(content);
                    }
                }
            }
        }
    }
    return success;
}


void Player::addAllLexicalData(InteractiveNoun *anObject){
    std::vector<std::string> nounAliases, verbAliases;

    if (anObject != nullptr){
        std::lock_guard<std::mutex> lexicalLock(lexicalMutex);
        nounAliases = anObject->getNounAliases();
        verbAliases = anObject->getVerbAliases();

        for (auto noun : nounAliases){
            inventoryLexicalData.addNoun(noun, anObject);
        }
        for (auto verb : verbAliases){
            inventoryLexicalData.addVerb(verb, anObject);
        }
    }
    
}


void Player::removeAllLexicalData(InteractiveNoun *anObject){
    std::vector<std::string> nounAliases, verbAliases;

    if (anObject != nullptr){
        std::lock_guard<std::mutex> lexicalLock(lexicalMutex);
        nounAliases = anObject->getNounAliases();
        verbAliases = anObject->getVerbAliases();

        for (auto noun : nounAliases){
            inventoryLexicalData.removeNoun(noun, anObject);
        }
        for (auto verb : verbAliases){
            inventoryLexicalData.removeVerb(verb, anObject);
        }
    }
}


bool Player::addNounAlias(std::string alias){
    bool success = false;
    Area *location = getLocation();

    std::lock_guard<std::mutex> lexicalLock(lexicalMutex);

    if (location != nullptr){
        location->registerAlias(false, alias, this);
    }
    success = InteractiveNoun::addNounAlias(alias);

    return success;
}


bool Player::removeNounAlias(std::string alias){
    bool success = false;
    Area *location = getLocation();

    std::lock_guard<std::mutex> lexicalLock(lexicalMutex);

    if (location != nullptr){
        location->unregisterAlias(false, alias, this);
    }
    success = InteractiveNoun::removeNounAlias(alias);

    return success;
}


bool Player::addVerbAlias(CommandEnum aCommand, std::string alias, parser::Grammar::Support direct, parser::Grammar::Support indirect, std::map<std::string, parser::PrepositionType> prepositions){
    bool success = false;
    Area *location = getLocation();

    std::lock_guard<std::mutex> lexicalLock(lexicalMutex);

    if (location != nullptr){
        location->registerAlias(true, alias, this);
    }
    success = InteractiveNoun::addVerbAlias(aCommand, alias, direct, indirect, prepositions);

    return success;
}


bool Player::removeVerbAlias(CommandEnum aCommand, std::string alias){
    bool success = false;
    Area *location = getLocation();

    std::lock_guard<std::mutex> lexicalLock(lexicalMutex);

    if (location != nullptr){
        location->unregisterAlias(true, alias, this);
    }
    success = InteractiveNoun::removeVerbAlias(aCommand, alias);

    return success;
}


bool Player::registerAlias(bool isVerb, std::string alias, InteractiveNoun *anObject){
    std::lock_guard<std::mutex> lexicalLock(lexicalMutex);

    if (anObject != nullptr){
        if (isVerb){
            inventoryLexicalData.addVerb(alias, anObject);
        } else {
            inventoryLexicalData.addNoun(alias, anObject);
        }
        return true;
    }
    return false;
}


bool Player::unregisterAlias(bool isVerb, std::string alias, InteractiveNoun *anObject){
    std::lock_guard<std::mutex> lexicalLock(lexicalMutex);

    if (anObject != nullptr){
        if (isVerb){
            inventoryLexicalData.removeVerb(alias, anObject);
        } else {
            inventoryLexicalData.removeNoun(alias, anObject);
        }
        return true;
    }
    return false;
}


ObjectType Player::getObjectType() const{
    return ObjectType::PLAYER;
}


std::string Player::serialize(){
    return "";
}


Player* Player::deserialize(std::string){
    return nullptr; 
}


std::string Player::look(Player *aPlayer, std::vector<EffectType> *effects){
    std::string message = getName() + " is " + getDescription() + ".";
    std::string resultMsg;
    EffectType anEffect = EffectType::NONE;

    resultMsg = getTextAndEffect(CommandEnum::LOOK, anEffect);
    if (resultMsg.compare("false") != 0){
        message += resultMsg;
    }
    if (anEffect != EffectType::NONE){
        effects->push_back(anEffect);
    }

    return message;
}  


std::string Player::take(Player *aPlayer, Item *anItem, InteractiveNoun *aContainer, InteractiveNoun *aCharacters, std::vector<EffectType> *effects){
    std::string message = "";
    std::string resultMsg;
    EffectType anEffect = EffectType::NONE;
    bool success;

    success = addToInventory(anItem);
    if (success){
        resultMsg = getTextAndEffect(CommandEnum::TAKE, anEffect);
        if (resultMsg.compare("false") != 0){
            message += resultMsg;
        }
        if (anEffect != EffectType::NONE){
            effects->push_back(anEffect);
        }
    } else {
        message = "false";
    }

    return message;
}


std::string Player::equip(Player *aPlayer, Item *anItem, InteractiveNoun *aCharacter, std::vector<EffectType> *effects){
    std::string message = "";
    std::string strSuccess, resultMsg;
    EffectType anEffect = EffectType::NONE;
    bool success = false;

    if (anItem != nullptr){
        strSuccess = equipItem(anItem);

        if (strSuccess.compare("true") == 0){
            success = true;
        } else if (strSuccess.compare("false") == 0){
            message = "false";
        } else {
            success = true;
            message = "Unequipped the " + strSuccess + ". ";
        }
    }

    if (success){
        resultMsg += getTextAndEffect(CommandEnum::EQUIP, anEffect);
        if (resultMsg.compare("false") != 0){
            message += resultMsg;
        }
        if (anEffect != EffectType::NONE){
            effects->push_back(anEffect);
        }
    }

    return message;
}


std::string Player::unequip(Player *aPlayer, Item *anItem, InteractiveNoun *aCharacter, std::vector<EffectType> *effects){
    std::string message = "";
    std::string resultMsg;
    EffectType anEffect = EffectType::NONE;
    bool success = false;

    if (anItem != nullptr){
        success = unequipItem(anItem);
    }

    if (success){
        resultMsg += getTextAndEffect(CommandEnum::EQUIP, anEffect);
        if (resultMsg.compare("false") != 0){
            message += resultMsg;
        }
        if (anEffect != EffectType::NONE){
            effects->push_back(anEffect);
        }
    }

    return message;
}


std::string Player::transfer(Player *aPlayer, Item *anItem, InteractiveNoun *aCharacter, InteractiveNoun *destination, std::vector<EffectType> *effects){
    std::string message = "";
    std::string resultMsg;
    bool success = false;
    EffectType anEffect = EffectType::NONE;

    if (anItem != nullptr){
        if (aPlayer == this){
            // item is being removed from this player
            success = removeFromInventory(anItem);
        } else if ((destination != nullptr) && (destination->getID() == this->getID())){
            // item is being added to this player
            success = addToInventory(anItem);
        }
    }

    if (success){
        resultMsg += getTextAndEffect(CommandEnum::TRANSFER, anEffect);
        if (resultMsg.compare("false") != 0){
            message += resultMsg;
        }
        if (anEffect != EffectType::NONE){
            effects->push_back(anEffect);
        }
    }

    return message;
}


std::string Player::go(Player *aPlayer, Area *anArea, InteractiveNoun *character, std::vector<EffectType> *effects){
    std::string message = "";
    std::string resultMsg;
    EffectType anEffect = EffectType::NONE;

    if (anArea != nullptr){
        setLocation(anArea);

        // if the player is in conversation, end the conversation
        if (getInConversation() != nullptr){
            setInConversation(nullptr);
        }

        resultMsg += getTextAndEffect(CommandEnum::GO, anEffect);
        if (resultMsg.compare("false") != 0){
            message += resultMsg;
        }
        if (anEffect != EffectType::NONE){
            effects->push_back(anEffect);
        }
    }

    return message;
}


std::string Player::attack(Player *aPlayer, Item *anItem, SpecialSkill*, InteractiveNoun*, bool, std::vector<EffectType> *effects){
    std::string message = "";

    return message;
}


std::string Player::talk(Player *aPlayer, NonCombatant *aNPC, std::vector<EffectType> *effects){
    std::string message = "";
    std::string resultMsg;
    EffectType anEffect = EffectType::NONE;

    if (aNPC != nullptr){
        setInConversation(aNPC);

        resultMsg += getTextAndEffect(CommandEnum::TALK, anEffect);
        if (resultMsg.compare("false") != 0){
            message += resultMsg;
        }
        if (anEffect != EffectType::NONE){
            effects->push_back(anEffect);
        }
    }

    return message;
} 

// would be better to have NonCombatant do their own selling **************************************************
std::string Player::buy(Player *aPlayer, Item *anItem, std::vector<EffectType> *effects){
    std::string message = "";
    std::string resultMsg;
    bool success = false;
    int price;
    EffectType anEffect = EffectType::NONE;

    if (anItem != nullptr){
        // check price  
        price = anItem->getType()->getCost();
        if (price > getMoney()){
            // can't afford the item
            message = "fYou don't have enough money to buy the " + anItem->getName() + ".";
            return message;
        } else {
            // buy the item
            aPlayer->subtractMoney(price);
            aPlayer->getInConversation()->addMoney(price);
            aPlayer->getInConversation()->removeFromInventory(anItem);
            aPlayer->addToInventory(anItem);
            success = true;
            message = "You bought the " + anItem->getName() + " for " + std::to_string(price) + " money.";
        }
    }

    if (success){
        resultMsg = getTextAndEffect(CommandEnum::BUY, anEffect);
        if (resultMsg.compare("false") != 0){
            message += resultMsg;
        }
        if (anEffect != EffectType::NONE){
            effects->push_back(anEffect);
        }
    }

    return message;
}


// would be better to have NonCombatant do their own buying **************************************************
std::string Player::sell(Player *aPlayer, Item *anItem, std::vector<EffectType> *effects){
    std::string message = "";
    std::string resultMsg;
    bool success = false;
    int price;
    EffectType anEffect = EffectType::NONE;
    NonCombatant *aNPC = getInConversation();

    if (anItem != nullptr){
        // check price  
        price = anItem->getType()->getCost();
        if (price > aNPC->getMoney()){
            // NPC can't afford the item
            message = "f" + aNPC->getName() + " doesn't have enough money to buy the " + anItem->getName() + ".";
            return message;
        } else {
            // buy the item
            aNPC->subtractMoney(price);
            aPlayer->addMoney(price);
            aPlayer->removeFromInventory(anItem);
            aNPC->addToInventory(anItem);
            success = true;
            message = "You sold the " + anItem->getName() + " to " + aNPC->getName() + " for " + std::to_string(price) + " money.";
        }
    }

    if (success){
        resultMsg = getTextAndEffect(CommandEnum::BUY, anEffect);
        if (resultMsg.compare("false") != 0){
            message += resultMsg;
        }
        if (anEffect != EffectType::NONE){
            effects->push_back(anEffect);
        }
    }

    return message;
}


/*std::string Player::search(Player *aPlayer, std::vector<EffectType> *effects){
    std::string message = "";

    return message;
} */


std::string Player::useSkill(Player *aPlayer, SpecialSkill *aSkill, InteractiveNoun *character, Combatant *aRecipient, bool playerSkill, std::vector<EffectType> *effects){
    std::string message = "";

    return message;
} 


std::string Player::warp(Player *aPlayer, Area*){
    std::string message = "";

    return message;
} 


InteractiveNoun* Player::copy(){
    return nullptr;
}


bool Player::editAttribute(Player *aPlayer, std::string){
    return false;
}


bool Player::editWizard(Player *aPlayer){
    return false;
}


std::map<std::string, DataType> Player::getAttributeSignature(){
    std::map<std::string, DataType> signature;

    signature["size"] = DataType::CHARACTER_SIZE;
    signature["player class"] = DataType::PLAYER_CLASS_PTR;
    signature["username"] = DataType::STRING_TYPE;
    signature["file descriptor"] = DataType::INT_TYPE;
    signature["maximum health"] = DataType::INT_TYPE;
    signature["spawn location"] = DataType::AREA_PTR;
    signature["maximum special points"] = DataType::INT_TYPE;
    signature["name"] = DataType::STRING_TYPE;
    signature["description"] = DataType::STRING_TYPE;
    signature["money"] = DataType::INT_TYPE;
    signature["current location"] = DataType::AREA_PTR;
    signature["maximum inventory weight"] = DataType::INT_TYPE;

    return signature;
}

}}