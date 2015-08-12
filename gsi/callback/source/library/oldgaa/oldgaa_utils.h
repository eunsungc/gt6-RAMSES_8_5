/*
 * Copyright 1999-2006 University of Chicago
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


/**********************************************************************
 oldgaa_utils.h:

Description:
	This header file used internally by the oldgaa routines
**********************************************************************/
#ifndef OLDGAA_UTILS_H
#define OLDGAA_UTILS_H
#define MAX_STRING_SIZE  1024
#define QUOTING	         '\"'
#define WHITESPACE       ' '
#define TAB              '\t'

/**********************************************************************
 *
 * Function Prototypes
 *
 **********************************************************************/

/**********************************************************************
  String Handling
 **********************************************************************/

int 
oldgaa_strings_match(const char *string1,
		  const char *string2);
char *
oldgaa_strcopy(const char *s, char *r);


/**********************************************************************
  Compare elements
 **********************************************************************/
int
oldgaa_compare_principals(oldgaa_principals_ptr element, oldgaa_principals_ptr new);

int
oldgaa_compare_rights(oldgaa_rights_ptr element, oldgaa_rights_ptr new);

int
oldgaa_compare_conditions(oldgaa_conditions_ptr element, oldgaa_conditions_ptr new);

int
oldgaa_compare_sec_attrbs(oldgaa_sec_attrb_ptr element, 
                       oldgaa_sec_attrb_ptr new);

/**********************************************************************
  Add new element to a list
 **********************************************************************/

oldgaa_principals_ptr
oldgaa_add_principal(oldgaa_policy_ptr *list, oldgaa_principals_ptr new);

oldgaa_rights_ptr
oldgaa_add_rights(oldgaa_rights_ptr *list, oldgaa_rights_ptr new);

oldgaa_cond_bindings_ptr
oldgaa_add_cond_binding(oldgaa_cond_bindings_ptr* list, oldgaa_cond_bindings_ptr new);

oldgaa_conditions_ptr
oldgaa_add_condition(oldgaa_conditions_ptr* list, oldgaa_conditions_ptr new);

oldgaa_sec_attrb_ptr
oldgaa_add_attribute(oldgaa_sec_attrb_ptr *list, oldgaa_sec_attrb_ptr new);

/**********************************************************************
  Bindings
 **********************************************************************/

int
oldgaa_bind_rights_to_principals(oldgaa_principals_ptr start, oldgaa_rights_ptr rights);

void
oldgaa_bind_rights_to_conditions(oldgaa_rights_ptr start, oldgaa_cond_bindings_ptr cond_bind);


/**********************************************************************
  Regex Handling Functions
 **********************************************************************/

int
oldgaa_check_reg_expr(char  *reg_expr, 
                  char **reg_expr_list);
int
oldgaa_regex_matches_string(const char * const  string,
                         const char * const  regex);

char **
oldgaa_parse_regex(char * str);

#endif /* OLDGAA_UTILS_H */
