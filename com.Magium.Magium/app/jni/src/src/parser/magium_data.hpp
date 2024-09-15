#pragma once

#include "magium_structs.hpp"

namespace MagiumSDL::Parser {
    class MagiumData{
    private:
        std::vector<VarVal> m_variables;

    public:
        bool exists(std::string var) {
            for (VarVal &v : m_variables) {
                if (v.var == var)
                    return true;
            }
            return false;
        }

        std::string &getVal(std::string var) {
            for (VarVal &v : m_variables){
                if (v.var == var)
                    return v.val;
            }
            SDL_LogError(SDL_LOG_PRIORITY_ERROR, "Make sure var exists before calling getVal()!");
        }

        void addVarVal(VarVal varVal) {
            if (exists(varVal.var)){
                if (varVal.val.find("+") != varVal.val.npos){
                    std::string &val = getVal(varVal.var);
                    val = std::to_string(std::stoi(val) + std::stoi(varVal.val));
                }
                else{
                    getVal(varVal.var) = varVal.val;
                }
                return;
            }
            if (int pos = varVal.val.find("+") != varVal.val.npos)
                varVal.val.erase(pos, 1);
            m_variables.push_back(varVal); 
        }

        void clear() { m_variables.clear(); }

        std::vector<VarVal> getAll() { return m_variables; }
        
        bool evaluateVarVal(VarVal varVal, BoolOP op) {
            if (exists(varVal.var)){
                std::string &val = getVal(varVal.var);
                switch (op) {
                    case BoolOP::EQUAL:         return val == varVal.val;
                    case BoolOP::NOT_EQUAL:     return val != varVal.val;
                    case BoolOP::GREATER:       return std::stoi(val) > std::stoi(varVal.val);
                    case BoolOP::LESSER:        return std::stoi(val) < std::stoi(varVal.val);
                    case BoolOP::GREATER_EQUAL: return std::stoi(val) >= std::stoi(varVal.val);
                    case BoolOP::LESSER_EQUAL:  return std::stoi(val) <= std::stoi(varVal.val);
                }
            }
            else{
                return false;
            }
        }
    };
}