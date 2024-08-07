/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*
 * src/include/regex/regerrs.h
 */

{
	REG_OKAY, "REG_OKAY", "no errors detected"
},

{
	REG_NOMATCH, "REG_NOMATCH", "failed to match"
},

{
	REG_BADPAT, "REG_BADPAT", "invalid regexp (reg version 0.8)"
},

{
	REG_ECOLLATE, "REG_ECOLLATE", "invalid collating element"
},

{
	REG_ECTYPE, "REG_ECTYPE", "invalid character class"
},

{
	REG_EESCAPE, "REG_EESCAPE", "invalid escape \\ sequence"
},

{
	REG_ESUBREG, "REG_ESUBREG", "invalid backreference number"
},

{
	REG_EBRACK, "REG_EBRACK", "brackets [] not balanced"
},

{
	REG_EPAREN, "REG_EPAREN", "parentheses () not balanced"
},

{
	REG_EBRACE, "REG_EBRACE", "braces {} not balanced"
},

{
	REG_BADBR, "REG_BADBR", "invalid repetition count(s)"
},

{
	REG_ERANGE, "REG_ERANGE", "invalid character range"
},

{
	REG_ESPACE, "REG_ESPACE", "out of memory"
},

{
	REG_BADRPT, "REG_BADRPT", "quantifier operand invalid"
},

{
	REG_ASSERT, "REG_ASSERT", "\"cannot happen\" -- you found a bug"
},

{
	REG_INVARG, "REG_INVARG", "invalid argument to regex function"
},

{
	REG_MIXED, "REG_MIXED", "character widths of regex and string differ"
},

{
	REG_BADOPT, "REG_BADOPT", "invalid embedded option"
},

{
	REG_ETOOBIG, "REG_ETOOBIG", "regular expression is too complex"
},

{
	REG_ECOLORS, "REG_ECOLORS", "too many colors"
},

{
	REG_CANCEL, "REG_CANCEL", "operation cancelled"
},
