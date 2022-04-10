# Brief overview of Test Cases checked in this test suite

- One set of test cases is to check basic syntax errors, such as missing 
TOKEN\_TYPE\_STATEMENT\_END (;), or missing end block tokens (}), or missing
or mismatching quotation marks.
- Another test case is to check if the parser correctly allows for multiple 
tokens in a statement.
- Another is to check if the parser allows an empty block in the config.
- Checks if it allows for backslashed escaped characters
- checks if correct whitespace after a quotation.
- Last is to check if parser allows for empty config file (it should).
