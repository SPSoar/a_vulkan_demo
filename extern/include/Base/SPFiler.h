#ifndef S_SPFILEINFO_H_
#define S_SPFILEINFO_H_

#include "SPVariable.h"
#include "SPTimer.h"

class SPFiler
{
public:
    enum class Permissions : sp_uint_8
    {
        None      = 0,
        WriteOnly = 1,
        ReadOnly  = 2,
        ReadWrite = 3
    };
    SPFiler() = delete;
    SPFiler(const sp_string& fileName);
  

    sp_string file_path() const;
    sp_string dir_path() const;
    sp_string dir_path(const sp_string& dirName) const;
    sp_string file_name() const;
    sp_string file_name_pure() const;
    sp_wstring to_wstring() const;
    sp_string extension() const;
    sp_string complete_extension() const;
    sp_bool has_extension(const sp_char* ext) const;
    sp_bool exists() const;
    sp_bool is_readable() const;
    sp_bool is_writable() const;
    sp_bool set_permissions(Permissions);
    sp_bool is_file() const;
    sp_bool is_directory() const;
    sp_uint_64 size() const;
    SPTimer last_modified() const;
    SPTimer last_read() const;
    sp_bool create_file()const;
    sp_bool create_directory(void) const;
    std::vector<SPFiler> get_directory_content(void) const;
    std::vector<SPFiler> get_directory_content(const sp_string& suffix) const;
    sp_bool delete_directory(void) const;
    sp_bool delete_directory_recursive(void) const;
    sp_bool delete_file(void) const;
    sp_bool rename_file(const sp_string& newName);
    sp_bool copy_to(const sp_string& newName, sp_bool bOverwrite = true) const;
    static sp_string Get_Temp_File_Name(const sp_string& fileName = "", const sp_string& path = "");
    static const sp_string& Get_Temp_Path(void);
    static sp_bool Try_Make_Directory(const sp_string& path);
    static sp_bool Is_Dir_Exist(const sp_string& path);
    static sp_bool Copy_Directory_To(const sp_string& path, const sp_string& destPath);
    static sp_string Get_Current_Executable();
protected:
    void set_file(const sp_string& name);
protected:
    sp_string file_path_;
};

#endif 

