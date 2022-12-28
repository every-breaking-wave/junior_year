# Doc

## handle comments

When matches "/*", set comment level to 0, switch to COMMENT condition. In this situation, when matches "/\*", add comment level , when matches "\*/", minus comment level, if comment level equals 0, return to INITIAL condition.

## handle strings

When matches ", switch to STR condition, in this situation, when matches " again , return to INITIAL condition, and set string buf in this period to matched string.

I also considered some special cases such as "\\\\"  and "\\\\*".

##  handle error

When matches each token except  strings, mark the position of last token, if some unexpected token is met, print error message and illegal token.

## handle end-of-file

Use pattern << EOF >> provided by flexc++ when read the end-of-file, return 0.



