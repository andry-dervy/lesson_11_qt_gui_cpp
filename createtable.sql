CREATE TABLE tasks (
id INTEGER PRIMARY KEY AUTOINCREMENT,
nameTask TEXT,
descriptionTask TEXT,
progress INTEGER NOT NULL CHECK(progress > -1 AND progress < 11),
limitDate TEXT
);