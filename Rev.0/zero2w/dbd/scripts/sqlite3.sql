CREATE TABLE Controller (
    Controller          INTEGER PRIMARY KEY,
    EventIndex          INTEGER NOT NULL DEFAULT 0,
    RecordSpecialEvents INTEGER NOT NULL DEFAULT 0,
    Interlock           INTEGER NOT NULL DEFAULT 0);

CREATE TABLE Doors (
    Controller INTEGER NOT NULL,
    Door       INTEGER NOT NULL,
    Mode       INTEGER NOT NULL DEFAULT 3,
    Delay      INTEGER NOT NULL DEFAULT 5,
    PRIMARY KEY (Controller, Door));

CREATE TABLE Cards (
    id         INTEGER PRIMARY KEY,
    Controller INTEGER NOT NULL,
    Card       INTEGER NOT NULL,
    StartDate  DATE    NULL,
    EndDate    DATE    NULL,
    Door1      INTEGER NOT NULL DEFAULT 0,
    Door2      INTEGER NOT NULL DEFAULT 0,
    Door3      INTEGER NOT NULL DEFAULT 0,
    Door4      INTEGER NOT NULL DEFAULT 0,
    PIN        INTEGER NOT NULL DEFAULT 0,
    UNIQUE (Controller, Card));

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

