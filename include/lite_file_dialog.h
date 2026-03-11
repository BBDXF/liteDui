/**
 * lite_file_dialog.h - 文件对话框
 */

#pragma once

#include "lite_dialog.h"
#include "lite_input.h"
#include "lite_list.h"
#include "lite_button.h"
#include <filesystem>

namespace liteDui {

class LiteFileDialog;
using LiteFileDialogPtr = std::shared_ptr<LiteFileDialog>;

enum class FileDialogMode {
    OpenFile,       // 打开文件
    OpenFolder,     // 打开文件夹
    SaveFile        // 保存文件
};

/**
 * 文件项结构体
 */
struct FileItem {
    std::string name;
    std::string path;
    bool isDirectory;
    size_t size;
    
    FileItem() : isDirectory(false), size(0) {}
    FileItem(const std::string& n, const std::string& p, bool isDir, size_t s = 0)
        : name(n), path(p), isDirectory(isDir), size(s) {}
};

class LiteFileDialog : public LiteDialog {
public:
    LiteFileDialog();
    ~LiteFileDialog() override = default;
    
    void setMode(FileDialogMode mode);
    FileDialogMode getMode() const { return m_mode; }
    
    void setCurrentPath(const std::string& path);
    std::string getCurrentPath() const { return m_currentPath; }
    
    void setFileFilter(const std::string& filter);
    std::string getFileFilter() const { return m_fileFilter; }
    
    std::string getSelectedPath() const { return m_selectedPath; }
    std::vector<std::string> getSelectedPaths() const { return m_selectedPaths; }
    
    // 静态便捷方法
    static LiteFileDialogPtr openFile(const std::string& title = "打开文件", 
                                      const std::string& initialPath = "");
    static LiteFileDialogPtr openFolder(const std::string& title = "选择文件夹",
                                        const std::string& initialPath = "");
    static LiteFileDialogPtr saveFile(const std::string& title = "保存文件",
                                      const std::string& initialPath = "");
    
    void render(SkCanvas* canvas) override;
    void onMouseReleased(const MouseEvent& event) override;

protected:
    void buildUI();
    void refreshFileList();
    void navigateUp();
    void navigateToPath(const std::string& path);
    void onFileSelected(int index);
    void onFileDoubleClicked(int index);
    bool matchesFilter(const std::string& filename) const;
    
    FileDialogMode m_mode = FileDialogMode::OpenFile;
    std::string m_currentPath;
    std::string m_fileFilter;  // 例如: "*.txt;*.cpp;*.h"
    std::string m_selectedPath;
    std::vector<std::string> m_selectedPaths;
    
    // UI 组件
    std::shared_ptr<LiteInput> m_pathInput;
    std::shared_ptr<LiteList> m_fileList;
    std::shared_ptr<LiteInput> m_fileNameInput;
    std::shared_ptr<LiteButton> m_upButton;
    
    std::vector<FileItem> m_files;
    
    static constexpr float TOOLBAR_HEIGHT = 40.0f;
    static constexpr float FILENAME_HEIGHT = 40.0f;
};

} // namespace liteDui
