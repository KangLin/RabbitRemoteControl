-- History.sql
CREATE TABLE IF NOT EXISTS history (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    url TEXT NOT NULL,
    title TEXT NOT NULL,
    visit_time DATETIME DEFAULT CURRENT_TIMESTAMP,
    visit_count INTEGER DEFAULT 1,
    last_visit_time DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- 创建索引以提高查询效率
CREATE INDEX IF NOT EXISTS idx_history_url ON history(url);
CREATE INDEX IF NOT EXISTS idx_history_time ON history(visit_time);

-- bookmarks.sql
-- 收藏夹表
CREATE TABLE IF NOT EXISTS bookmarks (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    url TEXT NOT NULL,
    title TEXT NOT NULL,
    icon BLOB,
    icon_url TEXT,
    description TEXT,
    created_time DATETIME DEFAULT CURRENT_TIMESTAMP,
    modified_time DATETIME DEFAULT CURRENT_TIMESTAMP,
    visit_count INTEGER DEFAULT 0,
    last_visit_time DATETIME,
    favorite BOOLEAN DEFAULT 0,  -- 是否是最爱
    tags TEXT,                   -- 标签，逗号分隔
    folder_id INTEGER DEFAULT 0, -- 文件夹ID
    FOREIGN KEY (folder_id) REFERENCES bookmark_folders(id) ON DELETE SET NULL
);

-- 收藏夹文件夹表
CREATE TABLE IF NOT EXISTS bookmark_folders (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    parent_id INTEGER DEFAULT 0,
    sort_order INTEGER DEFAULT 0,
    created_time DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- 创建索引
CREATE INDEX IF NOT EXISTS idx_bookmarks_url ON bookmarks(url);
CREATE INDEX IF NOT EXISTS idx_bookmarks_folder ON bookmarks(folder_id);
CREATE INDEX IF NOT EXISTS idx_bookmarks_favorite ON bookmarks(favorite);
CREATE INDEX IF NOT EXISTS idx_bookmarks_created ON bookmarks(created_time);

-- 插入默认文件夹
INSERT OR IGNORE INTO bookmark_folders (id, name, parent_id, sort_order) VALUES
    (1, '收藏夹栏', 0, 1),
    (2, '其他收藏夹', 0, 2),
    (3, '最爱', 1, 1),
    (4, '常用网站', 1, 2);
