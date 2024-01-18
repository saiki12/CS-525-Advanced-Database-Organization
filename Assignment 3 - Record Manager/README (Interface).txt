OPTIONAL EXTENSION - COMMAND LINE INTERFACE(CLI) README for RECORD MANAGER - Group 31
=====================================================================================

This is the Command Line Interface Readme file for the Record Manager where the user can create, and open the table, insert, update, and delete the records and close the database table by interacting with this interface via command line.

EXECUTION INSTRUCTIONS
----------------------

1) Go to Project root using Terminal.
2) Give ls command to list the files and check to ensure we are in the correct directory.
3) Give "make clean_i" to delete old compiled .o files.
4) Give "make interface" to compile all project files pertaining to the Command Line Interface. 
5) Give "make run_i" to run the CLI.

IMPLEMENTED FUNCTIONS IN THE INTERFACE:
---------------------------------------

1) CREATE <table_name>
--> Creates a new table in the database with the table name <table_name>

2) OPEN <table_name>
--> Opens the created table <table_name> in the first step, if there are no tables created then it simply throws an error.

3) INSERT <values>
--> Inserts values into the table record (also known as tuples in RDBMS).

4) UPDATE <rid> <values>
--> Updates the record values in the table <table_name>

5) DELETE <rid>
--> Deletes the given <rid> record values in the table <table_name>

6) CLOSE <table_name>
--> Closes the open table <table_name>, if there are no tables open then it gives an error saying 'No table is currently open'

7) EXIT
--> Exits the Command Line Interface.