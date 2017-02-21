/*********************************************************************//**
 * \author      Rachel Weissman-Hohler
 * \created     02/09/2017
 * \modified    02/20/2017
 * \course      CS467, Winter 2017
 * \file        CombatantType.cpp
 *
 * \details     Header file for CombatantType base class. Defines the members 
 *              and functions that apply to all combatant types. This class
 *              should not be instantiated.
 ************************************************************************/

#include "SpecialSkill.hpp"
#include "CombatantType.hpp"

namespace legacymud { namespace engine {

CombatantType::CombatantType()
: InteractiveNoun()
, name("")
, specialSkill(nullptr)
, attackBonus(0)
, armorBonus(0)
, resistantTo(DamageType::NONE)
, weakTo(DamageType::NONE)
, healPoints(0.1)
{ }


CombatantType::CombatantType(std::string name, SpecialSkill* skill, int attackBonus, int armorBonus, DamageType resistantTo, DamageType weakTo, float healPoints)
: InteractiveNoun()
, name(name)
, specialSkill(skill)
, attackBonus(attackBonus)
, armorBonus(armorBonus)
, resistantTo(resistantTo)
, weakTo(weakTo)
, healPoints(healPoints)
{ }


/*CombatantType::CombatantType(const CombatantType &otherCombatantType){

}


CombatantType & CombatantType::operator=(const CombatantType &otherCombatantType){

}


CombatantType::~CombatantType(){

}*/


std::string CombatantType::getName() const{
    std::lock_guard<std::mutex> nameLock(nameMutex);
    return name;
}


SpecialSkill* CombatantType::getSpecialSkill(){
    std::lock_guard<std::mutex> specialSkillLock(specialSkillMutex);
    return specialSkill;
}


int CombatantType::getAttackBonus(){
    return attackBonus.load();
}


int CombatantType::getArmorBonus(){
    return armorBonus.load();
}


DamageType CombatantType::getResistantTo(){
    return resistantTo.load();
}


DamageType CombatantType::getWeakTo(){
    return weakTo.load();
}


float CombatantType::getHealPoints(){
    return healPoints.load();
}


bool CombatantType::setName(std::string name){
    std::lock_guard<std::mutex> nameLock(nameMutex);
    this->name = name;
    return true;
}


bool CombatantType::setSpecialSkill(SpecialSkill* skill){
    return false;
}


bool CombatantType::setAttackBonus(int bonus){
    return false;
}


bool CombatantType::setArmorBonus(int bonus){
    return false;
}


bool CombatantType::setResistantTo(DamageType type){
    return false;
}


bool CombatantType::setWeakTo(DamageType type){
    return false;
}


bool CombatantType::setHealPoints(float healPoints){
    return false;
}


std::string CombatantType::serialize(){
    return "";
}


bool CombatantType::deserialize(std::string){
    return false;
}

}}