-- 创建表
CREATE TABLE favorite_folders (
    id INTEGER PRIMARY KEY AUTO_INCREMENT,
    name TEXT NOT NULL,
    parent_id INTEGER DEFAULT 0,
    sort_order INTEGER DEFAULT 0,
    created_time DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- 创建索引
CREATE INDEX idx_favorite_folders_parent ON favorite_folders(parent_id);

-- 创建表
CREATE TABLE favorite_tree (
    id INTEGER PRIMARY KEY AUTO_INCREMENT,
    name TEXT,
    `key` INTEGER DEFAULT 0,
    created_time DATETIME DEFAULT CURRENT_TIMESTAMP,
    modified_time DATETIME DEFAULT CURRENT_TIMESTAMP,
    last_visit_time DATETIME,
    parent_id INTEGER DEFAULT 0
);

-- 创建索引
CREATE INDEX idx_favorite_tree_key ON favorite_tree(`key`);
CREATE INDEX idx_favorite_tree_parent_id ON favorite_tree(parent_id);

-- 创建表
CREATE TABLE icon (
    id INTEGER PRIMARY KEY AUTO_INCREMENT,
    name TEXT,
    hash TEXT,
    data LONGBLOB,
    UNIQUE KEY uk_icon_name (name(255))
);

-- 创建索引
CREATE INDEX idx_icon_hash ON icon(hash(255));
CREATE INDEX idx_icon_name ON icon(name(255));

-- 创建表
CREATE TABLE recent (
    id INTEGER PRIMARY KEY AUTO_INCREMENT,
    operate_id TEXT NOT NULL,
    icon INTEGER DEFAULT 0,
    name TEXT NOT NULL,
    protocol TEXT,
    `type` TEXT,
    file TEXT NOT NULL,
    time DATETIME DEFAULT CURRENT_TIMESTAMP,
    description TEXT,
    UNIQUE KEY uk_recent_file (file(255))
);

-- 创建索引
CREATE INDEX idx_recent_file ON recent(file(255));

-- 创建表
CREATE TABLE favorite (
    id INTEGER PRIMARY KEY AUTO_INCREMENT,
    name TEXT NOT NULL,
    icon INTEGER DEFAULT 0,
    file TEXT NOT NULL,
    description TEXT,
    UNIQUE KEY uk_favorite_file (file(255))
);

-- 创建索引
CREATE INDEX idx_favorite_file ON favorite(file(255));

-- 首先设置分隔符
DELIMITER $$

-- 删除已存在的触发器
DROP TRIGGER IF EXISTS delete_icon_after_favorite$$

-- 创建favorite表的触发器
CREATE TRIGGER delete_icon_after_favorite
AFTER DELETE ON favorite
FOR EACH ROW
BEGIN
    DECLARE favorite_count INT DEFAULT 0;
    DECLARE recent_count INT DEFAULT 0;
    
    IF OLD.icon != 0 THEN
        -- 统计favorite表中引用该icon的数量
        SELECT COUNT(*) INTO favorite_count 
        FROM favorite 
        WHERE icon = OLD.icon;
        
        -- 统计recent表中引用该icon的数量
        SELECT COUNT(*) INTO recent_count 
        FROM recent 
        WHERE icon = OLD.icon;
        
        -- 如果都没有引用，则删除icon
        IF favorite_count = 0 AND recent_count = 0 THEN
            DELETE FROM icon WHERE id = OLD.icon;
        END IF;
    END IF;
END$$

-- 删除已存在的触发器
DROP TRIGGER IF EXISTS delete_icon_after_recent$$

-- 创建recent表的触发器
CREATE TRIGGER delete_icon_after_recent
AFTER DELETE ON recent
FOR EACH ROW
BEGIN
    DECLARE favorite_count INT DEFAULT 0;
    DECLARE recent_count INT DEFAULT 0;
    
    IF OLD.icon != 0 THEN
        -- 统计favorite表中引用该icon的数量
        SELECT COUNT(*) INTO favorite_count 
        FROM favorite 
        WHERE icon = OLD.icon;
        
        -- 统计recent表中引用该icon的数量
        SELECT COUNT(*) INTO recent_count 
        FROM recent 
        WHERE icon = OLD.icon;
        
        -- 如果都没有引用，则删除icon
        IF favorite_count = 0 AND recent_count = 0 THEN
            DELETE FROM icon WHERE id = OLD.icon;
        END IF;
    END IF;
END$$

-- 恢复分隔符
DELIMITER ;
