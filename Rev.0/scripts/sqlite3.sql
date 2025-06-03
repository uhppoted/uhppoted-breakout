CREATE TABLE Events (
    EventID      INTEGER PRIMARY KEY AUTOINCREMENT,
    Timestamp    DATETIME NULL,
    Type         INTEGER  NULL,
    Granted      INTEGER  NULL,
    Door         INTEGER  NULL,
    Direction    INTEGER  NULL,
    CardNumber   INTEGER  NULL,
    Reason       INTEGER  NULL);

CREATE TABLE EventIndex (
    Controller INTEGER PRIMARY KEY,
    EventIndex INTEGER DEFAULT 0);
