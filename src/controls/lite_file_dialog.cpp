/**
 * lite_file_dialog.cpp - 文件对话框实现
 */

#include "lite_file_dialog.h"
#include "lite_label.h"
#include "include/core/SkCanvas.h"
#include <algorithm>
#include <cstring>

namespace fs = std::filesystem;

namespace liteDui {

LiteFileDialog::LiteFileDialog() {
    setDialogSize(700.0f, 500.0f);
    setTitle("选择文件");
    setStandardButtons(Ok | Cancel);
    
    m_currentPath = fs::current_path().string();
    
    buildUI();
}

void LiteFileDialog::setMode(FileDialogMode mode) {
    m_mode = mode;
    if (mode == FileDialogMode::OpenFolder) {
        setTitle("选择文件夹");
    } else if (mode == FileDialogMode::SaveFile) {
        setTitle("保存文件");
    } else {
        setTitle("打开文件");
    }
    markDirty();
}

void LiteFileDialog::setCurrentPath(const std::string& path) {
    if (fs::exists(path)) {
        m_currentPath = fs::absolute(path).string();
        if (m_pathInput) {
            m_pathInput->setText(m_currentPath);
        }
        refreshFileList();
    }
}

void LiteFileDialog::setFileFilter(const std::string& filter) {
    m_fileFilter = filter;
    refreshFileList();
}

LiteFileDialogPtr LiteFileDialog::openFile(const std::string& title, const std::string& initialPath) {
    auto dialog = std::make_shared<LiteFileDialog>();
    dialog->setMode(FileDialogMode::OpenFile);
    dialog->setTitle(title);
    if (!initialPath.empty()) {
        dialog->setCurrentPath(initialPath);
    }
    return dialog;
}

LiteFileDialogPtr LiteFileDialog::openFolder(const std::string& title, const std::string& initialPath) {
    auto dialog = std::make_shared<LiteFileDialog>();
    dialog->setMode(FileDialogMode::OpenFolder);
    dialog->setTitle(title);
    if (!initialPath.empty()) {
        dialog->setCurrentPath(initialPath);
    }
    return dialog;
}

LiteFileDialogPtr LiteFileDialog::saveFile(const std::string& title, const std::string& initialPath) {
    auto dialog = std::make_shared<LiteFileDialog>();
    dialog->setMode(FileDialogMode::SaveFile);
    dialog->setTitle(title);
    if (!initialPath.empty()) {
        dialog->setCurrentPath(initialPath);
    }
    return dialog;
}

void LiteFileDialog::buildUI() {
    auto content = std::make_shared<LiteContainer>();
    content->setFlexDirection(FlexDirection::Column);
    content->setPadding(EdgeInsets::All(12));
    content->setGap(8);
    
    auto toolbar = std::make_shared<LiteContainer>();
    toolbar->setFlexDirection(FlexDirection::Row);
    toolbar->setHeight(TOOLBAR_HEIGHT);
    toolbar->setGap(8);
    
    m_upButton = std::make_shared<LiteButton>("↑");
    m_upButton->setWidth(40);
    m_upButton->setHeight(TOOLBAR_HEIGHT);
    m_upButton->setOnClick([this](const MouseEvent&) {
        navigateUp();
    });
    toolbar->addChild(m_upButton);
    
    m_pathInput = std::make_shared<LiteInput>();
    m_pathInput->setText(m_currentPath);
    m_pathInput->setFlexGrow(1);
    m_pathInput->setHeight(TOOLBAR_HEIGHT);
    m_pathInput->setOnTextChanged([this](const std::string& text) {
        if (fs::exists(text) && fs::is_directory(text)) {
            navigateToPath(text);
        }
    });
    toolbar->addChild(m_pathInput);
    
    content->addChild(toolbar);
    
    m_fileList = std::make_shared<LiteList>();
    m_fileList->setFlexGrow(1);
    m_fileList->setBackgroundColor(Color::White());
    m_fileList->setBorder(EdgeInsets::All(1));
    m_fileList->setBorderColor(Color::fromRGB(200, 200, 200));
    m_fileList->setOnSelectionChanged([this](int index) {
        onFileSelected(index);
    });
    m_fileList->setOnItemDoubleClicked([this](int index) {
        onFileDoubleClicked(index);
    });
    content->addChild(m_fileList);
    
    if (m_mode == FileDialogMode::SaveFile) {
        auto filenameRow = std::make_shared<LiteContainer>();
        filenameRow->setFlexDirection(FlexDirection::Row);
        filenameRow->setHeight(FILENAME_HEIGHT);
        filenameRow->setGap(8);
        filenameRow->setAlignItems(Align::Center);
        
        auto label = std::make_shared<LiteLabel>("文件名:");
        label->setWidth(60);
        filenameRow->addChild(label);
        
        m_fileNameInput = std::make_shared<LiteInput>();
        m_fileNameInput->setFlexGrow(1);
        m_fileNameInput->setHeight(32);
        filenameRow->addChild(m_fileNameInput);
        
        content->addChild(filenameRow);
    }
    
    setContent(content);
    refreshFileList();
}

void LiteFileDialog::refreshFileList() {
    if (!m_fileList) return;
    
    m_fileList->clearItems();
    m_files.clear();
    
    if (!fs::exists(m_currentPath) || !fs::is_directory(m_currentPath)) {
        return;
    }
    
    try {
        for (const auto& entry : fs::directory_iterator(m_currentPath)) {
            std::string filename = entry.path().filename().string();
            
            if (filename[0] == '.') continue;
            
            bool isDir = entry.is_directory();
            
            if (!isDir && m_mode == FileDialogMode::OpenFolder) {
                continue;
            }
            
            if (!isDir && !m_fileFilter.empty() && !matchesFilter(filename)) {
                continue;
            }
            
            size_t size = 0;
            if (!isDir && entry.is_regular_file()) {
                size = entry.file_size();
            }
            
            FileItem item(filename, entry.path().string(), isDir, size);
            m_files.push_back(item);
        }
        
        std::sort(m_files.begin(), m_files.end(), [](const FileItem& a, const FileItem& b) {
            if (a.isDirectory != b.isDirectory) {
                return a.isDirectory;
            }
            return a.name < b.name;
        });
        
        for (const auto& file : m_files) {
            std::string displayName = file.isDirectory ? "[" + file.name + "]" : file.name;
            m_fileList->addItem(displayName);
        }
        
    } catch (const fs::filesystem_error&) {
    }
}

void LiteFileDialog::navigateUp() {
    fs::path currentPath(m_currentPath);
    if (currentPath.has_parent_path()) {
        fs::path parentPath = currentPath.parent_path();
        if (parentPath != currentPath) {
            navigateToPath(parentPath.string());
        }
    }
}

void LiteFileDialog::navigateToPath(const std::string& path) {
    if (fs::exists(path) && fs::is_directory(path)) {
        m_currentPath = fs::absolute(path).string();
        if (m_pathInput) {
            m_pathInput->setText(m_currentPath);
        }
        refreshFileList();
    }
}

void LiteFileDialog::onFileSelected(int index) {
    if (index < 0 || index >= static_cast<int>(m_files.size())) {
        return;
    }
    
    const FileItem& item = m_files[index];
    
    if (m_mode == FileDialogMode::SaveFile && !item.isDirectory) {
        if (m_fileNameInput) {
            m_fileNameInput->setText(item.name);
        }
    }
    
    m_selectedPath = item.path;
}

void LiteFileDialog::onFileDoubleClicked(int index) {
    if (index < 0 || index >= static_cast<int>(m_files.size())) {
        return;
    }
    
    const FileItem& item = m_files[index];
    
    if (item.isDirectory) {
        navigateToPath(item.path);
    } else {
        if (m_mode == FileDialogMode::OpenFile) {
            m_selectedPath = item.path;
            accept();
        }
    }
}

bool LiteFileDialog::matchesFilter(const std::string& filename) const {
    if (m_fileFilter.empty()) return true;
    
    std::string filter = m_fileFilter;
    size_t pos = 0;
    while ((pos = filter.find(';')) != std::string::npos) {
        std::string pattern = filter.substr(0, pos);
        filter.erase(0, pos + 1);
        
        if (pattern.find('*') != std::string::npos) {
            std::string ext = pattern.substr(pattern.find('*') + 1);
            if (filename.size() >= ext.size() &&
                filename.compare(filename.size() - ext.size(), ext.size(), ext) == 0) {
                return true;
            }
        } else if (filename == pattern) {
            return true;
        }
    }
    
    if (filter.find('*') != std::string::npos) {
        std::string ext = filter.substr(filter.find('*') + 1);
        if (filename.size() >= ext.size() &&
            filename.compare(filename.size() - ext.size(), ext.size(), ext) == 0) {
            return true;
        }
    } else if (filename == filter) {
        return true;
    }
    
    return false;
}

void LiteFileDialog::render(SkCanvas* canvas) {
    LiteDialog::render(canvas);
}

void LiteFileDialog::onMouseReleased(const MouseEvent& event) {
    if (m_mode == FileDialogMode::SaveFile && m_fileNameInput) {
        std::string filename = m_fileNameInput->getText();
        if (!filename.empty()) {
            fs::path fullPath = fs::path(m_currentPath) / filename;
            m_selectedPath = fullPath.string();
        }
    }
    
    LiteDialog::onMouseReleased(event);
}

} // namespace liteDui
