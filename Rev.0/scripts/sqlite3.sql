CREATE TABLE Events (
    Controller   INTEGER NOT NULL,
    EventID      INTEGER NOT NULL,
    Timestamp    DATETIME NULL,
    Type         INTEGER  NULL,
    Granted      INTEGER  NULL,
    Door         INTEGER  NULL,
    Direction    INTEGER  NULL,
    CardNumber   INTEGER  NULL,
    Reason       INTEGER  NULL,
    PRIMARY KEY (Controller, EventID));

CREATE TABLE Controller (
    Controller          INTEGER PRIMARY KEY,
    EventIndex          INTEGER DEFAULT 0,
    RecordSpecialEvents INTEGER DEFAULT 0);
