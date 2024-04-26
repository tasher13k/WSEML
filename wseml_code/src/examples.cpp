struct X { int a; double b[100]; };

X a, b, c;

{{"name", "a"}, {"name", "b"}, {"index", 15}}          &a.b[15]

{"direct", {{"name", "a"}, {"name", "b"}, {"index", 15}}}
{"stack", {"direct", {{"name", "a"}, {"name", "b"}}}}


<if-statement>::=<if-statement-with-else> | <if-statement-without-else>

void if_statement_with_else()
{
    match("if");
    match(condition);
    match(" then ");
    match(statement);
    match(" else ");
    match(statement);
    put(...)
}

void int_def()
{
    match("int ");
    match(ident);
    match(";");
    put(...);
}

a*b*c