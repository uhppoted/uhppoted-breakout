CREATE TABLE Controller (
    Controller          INTEGER PRIMARY KEY,
    EventIndex          INTEGER DEFAULT 0,
    RecordSpecialEvents INTEGER DEFAULT 0);

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

CREATE TABLE Cards (
    id         INTEGER PRIMARY KEY,
    Controller INTEGER NOT NULL,
    Card       INTEGER NOT NULL,
    StartDate  DATE NULL,
    EndDate    DATE NULL,
    Door1      INTEGER DEFAULT 0,
    Door2      INTEGER DEFAULT 0,
    Door3      INTEGER DEFAULT 0,
    Door4      INTEGER DEFAULT 0,
    PIN        INTEGER DEFAULT 0,
    UNIQUE (Controller, Card));
