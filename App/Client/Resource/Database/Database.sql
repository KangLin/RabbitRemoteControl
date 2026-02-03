-- 创建表
CREATE TABLE favorite_folders (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    parent_id INTEGER DEFAULT 0,
    sort_order INTEGER DEFAULT 0,
    created_time DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- 创建索引
CREATE INDEX idx_favorite_folders_parent ON favorite_folders(parent_id);

-- 创建表
CREATE TABLE favorite_tree (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT,
    key INTEGER DEFAULT 0,
    created_time DATETIME DEFAULT CURRENT_TIMESTAMP,
    modified_time DATETIME DEFAULT CURRENT_TIMESTAMP,
    last_visit_time DATETIME,
    parent_id INTEGER DEFAULT 0
);

-- 创建索引
CREATE INDEX idx_favorite_tree_key ON favorite_tree(key);
CREATE INDEX idx_favorite_tree_parent_id ON favorite_tree(parent_id);

-- 创建表
CREATE TABLE icon (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT UNIQUE,
    hash TEXT,
    data BLOB
);

-- 创建索引
CREATE INDEX idx_icon_hash ON icon(hash);
CREATE INDEX idx_icon_name ON icon(name);

-- 创建表
CREATE TABLE recent (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    operate_id TEXT NOT NULL,
    icon INTEGER DEFAULT 0,
    name TEXT NOT NULL,
    protocol TEXT,
    type TEXT,
    file TEXT UNIQUE NOT NULL,
    time DATETIME DEFAULT CURRENT_TIMESTAMP,
    description TEXT
);

-- 创建索引
CREATE INDEX idx_recent_file ON recent(file);

-- 创建表
CREATE TABLE favorite (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    icon INTEGER DEFAULT 0,
    file TEXT UNIQUE NOT NULL,
    description TEXT
);

-- 创建索引
CREATE INDEX idx_favorite_file ON favorite(file);

-- 删除已存在的触发器（单独执行）
DROP TRIGGER IF EXISTS delete_icon_after_favorite;

-- 创建新的触发器（单独执行）
CREATE TRIGGER delete_icon_after_favorite
AFTER DELETE ON favorite
FOR EACH ROW
BEGIN
    DELETE FROM icon
    WHERE id = OLD.icon
      AND OLD.icon != 0
      AND NOT EXISTS (
          -- 检查 favorite 表中是否还有引用
          SELECT 1 FROM favorite WHERE icon = OLD.icon
      )
      AND NOT EXISTS (
          -- 检查 recent 表中是否还有引用
          SELECT 1 FROM recent WHERE icon = OLD.icon
      );
END;

-- 删除已存在的触发器（单独执行）
DROP TRIGGER IF EXISTS delete_icon_after_recent;
-- 为 recent 表也创建（单独执行）
CREATE TRIGGER delete_icon_after_recent
AFTER DELETE ON recent
FOR EACH ROW
BEGIN
    DELETE FROM icon
    WHERE id = OLD.icon
      AND OLD.icon != 0
      AND (SELECT COUNT(*) FROM favorite WHERE icon = OLD.icon) = 0
      AND (SELECT COUNT(*) FROM recent WHERE icon = OLD.icon) = 0;
END;
