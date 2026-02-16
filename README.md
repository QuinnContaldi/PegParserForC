# PegParserForC
## Libraries in use
1. https://github.com/thradams/cgrammar/blob/main/grammar.txt
2. https://github.com/yhirose/cpp-peglib

## What The Hell Is A PEG Parser
My boss gave me a seemingly monumental task. Make a PEG Parser for C23... Before building something you must understand what we are building. A good place to start is learning what a PEG Parser is. So lets start by describing some key atributes about PEG Parsers

### Deterministic
1. A PEG Parser defines exactly one result for any given input or output.
2. That means there is no ambiguity, multiple parse trees, it fails or parses the results are unique.
    - The parser never guesses and in never backtracks across choices. 
    - Once a rule succeds thats its final path. 
    - In a Context Free Grammars something like ```1+2*3``` could produce either results ```(1+2) * 3``` or ```1+(2*3)```.
    - However given the PEG structure we can eliminate that ambiguity of CFG's by the structure below.
    - ```
        Expr <- Sum
        Sum  <- Product ('+' Product)*
        Product <- Number ('*' Number)*
        ```
3. We always have one way of parsing something *NO* ambiguity is allowed

### Ordered
1. In a PEG parser the choice operator **/** tries the alternatives *left to right* and commits the first one that succeeds. **THIS IS NOT A LOGICAL OR**
2. Given the following ```A <- B / C```
    1. Try B
    2. If B _succeed_ -> stop
    3. C is never considered even if it would pass
    4. if B fails then stop C is not even considered
    5. **THERE IS NO _BACKTRACKING ONCE_ AN INPUT IS CONSUMED**
3. Example 1 ```Digit <- [0-9] / [0-9][0-9]
    - Input: 42
    1. [0-9] would match 4 Success Parser Commits
    2. Success -> Parser commits
    3. WOOOOOOOOOOW but 2 is left so the rule ends and parser fails later
    4. Even though  [0-9][0-9] would have worked it is not tried
    5. THE KEY POINT IS THAT ORDER MATTERS WITH PEG PARSERS
4. PEG Trades 
    - abiguity and exponential backtracking 
    - for predictability and linear-time parsing

### Greedy
1. PEG repetitions (*, +, ?) consume as much input as possible while still allowing the enclosing rule to succeed.
2. ```A <- B* C```
    1. Repeats B as long as it succeeds
    2. Only stops when B fails
    3. Then immediately tries to match C
    4. It does not backtrack to reduce B
3. Some Intresting Behavior to note A <- 'a'* 'a'
    - Input aaa
    1. 'a'* would consume all a's
    2. this would cause the entire parse to fail even though this would work by backing off one a
    - A corrected form 
    - ```A <- 'a'+```
    - ```A <- 'a' 'a'*```
4. In Regex engines this would work due to back tracking. However, in PEG you cant do backtracking.

### Common Misunderstandings
1. Wait so you try the next rule on failure but dont try the next choice on failure? Really the question you are asking is the difference between Case1 and Case2
- Case 1: Rule without choice
    ```
    A <- B* C
    B <- 'a'
    C <- 'x'
    ```
    - Input: aaax
    - How PEG executes this
    - Think in imperative terms:
    - parse_B_star();   // must succeed
    - parse_C();        // must succeed
    - Step 1: B*
    1. B matches 'a' → consume
    2. B matches 'a' → consume
    3. B matches 'a' → consume
    4. B sees 'x' → fails
    - IMPORTANT This failure does NOT fail B* It just means: “stop repeating” so B* succeeds, consuming "aaa".
    5. now the input is 'x' and we move onto rule x
    6. rule x succeds and the whole sequence is accepted.
    - ** THE MAIN IDEA**: In a sequence, failure of a sub-expression only matters if it causes the entire sequence to fail.
- Case 2: Parse with the choice operator right hand side is never reached
    ```
    A <- B* / C
    B <- 'a'
    C <- 'x'

    if (parse_B_star())
    {
        return success;   // COMMIT
    } 
    else
    {
        return parse_C();
    }
    ```
    1. B matches 'a' → consume
    2. B matches 'a' → consume
    3. B matches 'a' → consume
    4. Left Side Succeeded and Peg Commits
    5. PEG does not try C
- Case 3: Parse with choice operator left hand side fails right hand side is reached
- Input: 'x'
```
A <- B+ / C
B <- 'a'
C <- 'x'
if (parse_B_plus()) 
{
    return success;
} 
else
{
    return parse_C();
}
```
1. B+ fails since current char is 'x'
2. This is a real expression failure thus we revert and now try rule C. Notice the difference. It was not a succesful parse that hit a terminating character. This failed right off the bat.
4. C <- 'x'passes since current char is 'x'
5. consume 'x' and succedes via rule C
#### Case 1 Case 2 Case 3 Lessons 
- Case 1: A repetition stopping is not failure.
- Case 2: A sequence failing is failure.
- Case 3: Ordered choice only reacts to real failure.

### Peg Table Symbols and Meaning
#### Repetition Operators
| Symbol | Meaning                        | Example  | Explanation                        |
| ------ | ------------------------------ | -------- | ---------------------------------- |
| `*`    | Zero or more                   | `Digit*` | Matches `""`, `"1"`, `"123"`       |
| `+`    | One or more                    | `Digit+` | Matches `"1"`, `"123"` (not empty) |
| `?`    | Optional (zero or one)         | `Sign?`  | Matches `""` or `"+"`              |
| `{n}`  | Exactly *n* (rare / extension) | –        | PEG typically prefers `+ * ?`      |

##### Character Classes 
| Syntax        | Meaning                                             |
| ------------- | --------------------------------------------------- |
| `[0-9]`       | Any digit                                           |
| `[+-]`        | Either `'+'` or `'-'`                               |
| `[abc]`       | One of `a`, `b`, or `c`                             |
| `[^0-9]`      | Anything *except* digits                            |
| `.`           | Any single character                                |
| `[\t ]`       | A tab or a space                                    |
| `[[:space:]]` | Any whitespace character (implementation-dependent) |

#### Sequence (AND)
| Item                | Content                                  |
| ------------------- | ---------------------------------------- |
| **Syntax**          | `A B C`                                  |
| **Meaning**         | Match A, then B, then C                  |
| **Example**         | `abc`                                    |
| **Equivalent code** | `parse('('); parse(Number); parse(')');` |


#### Ordered Choice (OR)
| Item             | Content                                           |
| ---------------- | ------------------------------------------------- |
| **Syntax**       | `A / B`                                           |
| **Meaning**      | Try A; if it fails without consuming input, try B |
| **Example**      | `'+' / '-'`                                       |
| **Grammar rule** | `Sign <- '+' / '-'`                               |
| **Note**         | `"A" / "AB"` matches `"A"`, never `"AB"`          |

#### Grouping
| Item            | Content                                  |
| --------------- | ---------------------------------------- |
| **Syntax**      | `( … )`                                  |
| **Meaning**     | Group expressions                        |
| **Example**     | `(',' Number)*`                          |
| **Explanation** | Zero or more occurrences of `',' Number` |


#### Rule Definition (Nonterminals)
| Item        | Content                     |
| ----------- | --------------------------- |
| **Syntax**  | `Rule <- expression`        |
| **Meaning** | Define a grammar rule       |
| **Example** | `Number <- [0-9]+`          |
| **Concept** | Think of rules as functions |


#### Lookahead (VERY IMPORTANT)
| Item                   | Content                                       |
| ---------------------- | --------------------------------------------- |
| **Positive lookahead** | `&e` — must match, but does not consume input |
| **Negative lookahead** | `!e` — must NOT match                         |
| **Example rule**       | `Identifier <- [a-zA-Z]+ ![0-9]`              |
| **Matches**            | Letters not followed by a digit               |
| **Note**               | Lookahead does not consume input              |


#### Literal Strings
| Item               | Content                 |
| ------------------ | ----------------------- |
| **Syntax**         | `"abc"`                 |
| **Meaning**        | Match literal string    |
| **Syntax (char)**  | `'a'`                   |
| **Meaning (char)** | Match literal character |

#### Whitespace Handling (peglib-specific)
| Item            | Content                                          |
| --------------- | ------------------------------------------------ |
| **Directive**   | `%whitespace <- expr`                            |
| **Meaning**     | Automatically skipped between tokens             |
| **Rule type**   | Silent rule (does not appear in parse tree)      |
| **Example**     | `%whitespace <- [\t ]*`                          |
| **Explanation** | Skip any number of spaces or tabs between tokens |

### Rules, Sequence, Choice, Repetition
> Peg Parsers have similar functionality to its imperative counterparts
#### Rules are Functions
- Think of rules as functions when you write something like this
- `Number <- [0-9]+
- this is the same idea as the imperative version ``` bool number(){ return matchDigitOneOrMore();}
- Rules behave exactly like function
- Success = return true
- Failure = return false
- input cursor moves forwar on success consuming charaters
#### Sequence -> Function Calls
- A <- B C D
```
 bool A() 
{
    if (!B()) return false;
    if (!C()) return false;
    if (!D()) return false;
    return true;
}
```
- all parts must succeed
- failure stops immediately
- Cursor advances step-by-step
#### Choice if / else
- A <- B / C
```
bool A() 
{
    if (B())
    {
        return true;
    }
    return C();
}
```
- if B() succeeds C() is never called
- No backtracking after succes
#### Repetition (*, +, ?) → Loops
- A <- B*
```
bool A()
{
    while (B()) 
    {
        // keep consuming
    }
    return true;
}
```
- Failure stops the loop
- Failure does not consume input
- Loop itself always succeeds

## The Difference between A recursive decent parser and a PEG parser
Now that we have at least a baseline familiarity with PEG Parsers lets discuss the difference in detail between the C recursive decent parser and the C PEG Parser. Pay special attention to 
### Left recursion
- "A rule is left-recursive if it can call itself before consuming input."
- `A → A α`
- This is also not allowed
```
A → B α
B → A β
```
The basic idea is that Recursive-descent and PEG cannot handle this because:
    1. Parser calls A
    2. A immediately calls A
    3. No input consumed
    4. Infinite recursion
    **This is not about precedence This is about call order vs input consumption. We get stuck in an infinite loop**
#### A comparision of C Grammers Recursive Decent Parser
Take a look at this broken grammar and tell me whats wrong with it.
```
expression
    : assignment_expression
    | expression ',' assignment_expression
    ;
```
You most likely noticed `expression → expression ',' assignment_expression` is the dreaded left recursion we were just talking about. Your function may look something like the following
```
parse_expression() 
{
    parse_expression(); // ← infinite loop
    match(',');
    parse_assignment_expression();
}
```
#### How PEG could address this issue
- Now look at our PEG Grammar `Expression <- AssignmentExpression (',' AssignmentExpression)*`
- No call to Expression without consuming input
