/**
 *  Function.cpp
 *
 *  Implementation for the function class
 *
 *  @author Emiel Bruijntjes <emiel.bruijntjes@copernica.com>
 *  @copyright 2013 Copernica BV
 */
#include "includes.h"

/**
 *  Set up namespace
 */
namespace Php {

/**
 *  Function that is called by the Zend engine every time that a function gets called
 *  @param  ht      
 *  @param  return_value
 *  @param  return_value_ptr
 *  @param  this_ptr
 *  @param  return_value_used
 *  @param  tsrm_ls
 *  @return integer
 */
void invoke_function(INTERNAL_FUNCTION_PARAMETERS)
{
    // find the function name
    const char *name = get_active_function_name(TSRMLS_C);
    
    // uncover the hidden pointer inside the function name
    Function *function = HiddenPointer<Function>(name);

    // wrap the return value
    Value ret(return_value, true);

    // construct parameters
    Parameters params(ZEND_NUM_ARGS());

    // call the appropriate object
    ret = function->invoke(params);
}

/**
 *  Fill a function entry
 * 
 *  This method is called when the extension is registering itself, when the 
 *  function or method introces himself
 * 
 *  @param  name        Name of the function
 *  @param  entry       Entry to be filled
 */
void Function::fill(const char *name, zend_function_entry *entry)
{
    // fill the members of the entity, and hide a pointer to the current object in the name
    entry->fname = HiddenPointer<Function>(this, name);
    entry->handler = invoke_function;
    entry->arg_info = _arguments->internal();
    entry->num_args = _arguments->argc();

    // there are no flags like deprecated, private or protected
    entry->flags = 0;
    
    // we should fill the first argument as well
    fill(name, (zend_internal_function_info *)entry->arg_info);
}

/**
 *  Fill a function entry
 *  @param  name        Name of the function
 *  @param  info        Info to be filled
 */
void Function::fill(const char *name, zend_internal_function_info *info)
{
    // fill in all the members, note that return reference is false by default,
    // because we do want to return references, inside the name we hide a pointer
    // to the current object
    info->_name = HiddenPointer<Function>(this, name);
    info->_name_len = strlen(name);
    info->_class_name = NULL;
    
    // number of required arguments, and the expected return type
    info->required_num_args = _arguments->required();
    info->_type_hint = _type;
    
    // we do not support return-by-reference
    info->return_reference = false;
    
    // passing by reference is not used
    info->pass_rest_by_reference = false;
}

/**
 *  End of namespace
 */
}

