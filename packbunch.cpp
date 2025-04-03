#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <vector>
#include <cstdio>
#include <cctype>
#include <cstdlib>

namespace pb
{
    constexpr int SUCCESS =  0;
    constexpr int FAILURE = -1;

    constexpr char VERSION[] = "1.0";

    std::string path;

    void help();
    void list();
    int view_bunch(const std::string& bunch_name);
    int create_bunch(const std::string& bunch_name);
    int delete_bunch(const std::string& bunch_name);
    int add_package(const std::string& bunch_name, const std::string& package_name);
    int remove_package(const std::string& bunch_name, const std::string& package_name);
    int install_bunch(const std::string& bunch_name);
    int uninstall_bunch(const std::string& bunch_name);
    int import_bunch(const std::string& bunch_path);
    int export_bunch(const std::string& bunch_name, const std::string& export_path);

    bool valid_bunch_name(const std::string& name);
    bool valid_package_name(const std::string& name);
}

int main(int argc, const char* argv[])
{
    char *sudo = std::getenv("SUDO_USER");
    if (sudo)
    {
        pb::path = "/home/" + std::string (sudo) + "/.packbunch/bunches/";
    }
    else
    {
        char *home_path = std::getenv("HOME");
        if (!home_path)
        {
            std::cerr << "Couldn't get path to home directory.\n";
            return pb::FAILURE;
        }
        pb::path = std::string {home_path} + "/.packbunch/bunches/";
    }
    std::filesystem::path fs_path {pb::path};
    if (!std::filesystem::exists(fs_path))
    {
        if (std::filesystem::create_directories(fs_path))
        {
            std::cout << "Bunch directory didn't exist and was created.\n";
        }
        else
        {
            std::cerr << "Bunch directory doesn't exist and couldn't be created. Try manually creating a \".packbunch\" directory within your home directory, then a \"bunches\" directory inside that.\n";
            return pb::FAILURE;
        }
    }
    if (argc <= 1)
    {
        pb::help();
        return pb::SUCCESS;
    }
    std::string command_name {argv[1]};
    if (command_name == "help" || command_name == "--help")
    {
        pb::help();
        return pb::SUCCESS;
    }
    if (command_name == "version" || command_name == "--version")
    {
        std::cout << "packbunch " << pb::VERSION << '\n';
        return pb::SUCCESS;
    }
    if (command_name == "list")
    {
        pb::list();
        return pb::SUCCESS;
    }
    if (command_name == "view")
    {
        if (argc <= 2)
        {
            std::cerr << "No bunch name provided.\nUsage: packbunch view <bunch>\n";
            return pb::FAILURE;
        }
        std::string bunch_name {argv[2]};
        return pb::view_bunch(bunch_name);
    }
    if (command_name == "create")
    {
        if (argc <= 2)
        {
            std::cerr << "No bunch name provided.\nUsage: packbunch create <name>\n";
            return pb::FAILURE;
        }
        std::string bunch_name {argv[2]};
        return pb::create_bunch(bunch_name);
    }
    if (command_name == "delete")
    {
        if (!sudo)
        {
            std::cerr << "The \"delete\" command must be run using sudo.\n";
            return pb::FAILURE;
        }
        if (argc <= 2)
        {
            std::cerr << "No bunch name provided.\nUsage: packbunch delete <name>\n";
            return pb::FAILURE;
        }
        std::string bunch_name {argv[2]};
        std::cout << "Do you want to uninstall the packages in bunch \"" << bunch_name << "\" before deleting it? (y/n): ";
        std::string option {};
        std::getline(std::cin, option);
        if (option != "n")
            pb::uninstall_bunch(bunch_name);
        return pb::delete_bunch(bunch_name); 
    }
    if (command_name == "add")
    {
        if (argc <= 2)
        {
            std::cerr << "No bunch name provided.\nUsage: packbunch add <bunch> <package>...\n";
            return pb::FAILURE;
        }
        if (argc <= 3)
        {
            std::cerr << "No package names provided.\nUsage: packbunch add <bunch> <package>...\n";
            return pb::FAILURE;
        }
        std::string bunch_name {argv[2]};
        int status = pb::SUCCESS;
        for (int i = 3; i < argc; i++)
        {
            if (pb::add_package(bunch_name, std::string {argv[i]}) == pb::FAILURE)
                status = pb::FAILURE;
        }
        return status;
    }
    if (command_name == "remove")
    {
        if (argc <= 2)
        {
            std::cerr << "No bunch name provided.\nUsage: packbunch remove <bunch> <package>...\n";
            return pb::FAILURE;
        }
        if (argc <= 3)
        {
            std::cerr << "No package names provided.\nUsage: packbunch remove <bunch> <package>...\n";
            return pb::FAILURE;
        }
        std::string bunch_name {argv[2]};
        int status = pb::SUCCESS;
        for (int i = 3; i < argc; i++)
        {
            if (pb::remove_package(bunch_name, std::string {argv[i]}) == pb::FAILURE)
                status = pb::FAILURE;
        }
        return status;
    }
    if (command_name == "install")
    {
        if (!sudo)
        {
            std::cerr << "The \"install\" command must be run using sudo.\n";
            return pb::FAILURE;
        }
        if (argc <= 2)
        {
            std::cerr << "No bunch name provided.\nUsage: packbunch install <bunch>\n";
            return pb::FAILURE;
        }
        std::string bunch_name {argv[2]};
        return pb::install_bunch(bunch_name); 
    }
    if (command_name == "uninstall")
    {
        if (!sudo)
        {
            std::cerr << "The \"uninstall\" command must be run using sudo.\n";
            return pb::FAILURE;
        }
        if (argc <= 2)
        {
            std::cerr << "No bunch name provided.\nUsage: packbunch uninstall <bunch>\n";
            return pb::FAILURE;
        }
        std::string bunch_name {argv[2]};
        return pb::uninstall_bunch(bunch_name); 
    }
    if (command_name == "import")
    {
        if (argc <= 2)
        {
            std::cerr << "No bunch path provided.\nUsage: packbunch import <path>\n";
            return pb::FAILURE;
        }
        std::string bunch_path {argv[2]};
        return pb::import_bunch(bunch_path);
    }
    if (command_name == "export")
    {
        if (argc <= 2)
        {
            std::cerr << "No bunch name provided.\nUsage: packbunch export <bunch> <path>\n";
            return pb::FAILURE;
        }
        if (argc <= 3)
        {
            std::cerr << "No export path provided.\nUsage: packbunch export <bunch> <path>\n";
            return pb::FAILURE;
        }
        std::string bunch_name {argv[2]};
        std::string export_path {argv[3]};
        return pb::export_bunch(bunch_name, export_path);
    }

    std::cerr << "Command \"" << command_name << "\" doesn't exist. Use \"packbunch help\" to see all available commands.\n";
    return pb::FAILURE;
}

void pb::help()
{
    std::cout <<
    "Usage:\n"
    "  packbunch help                         Shows this.\n"
    "  packbunch version                      Shows the program's version.\n"
    "  packbunch list                         Lists all bunches.\n"
    "  packbunch view <bunch>                 Lists all packages in bunch.\n"
    "  packbunch create <name>                Creates a bunch with the specified name.\n"
    "  packbunch delete <bunch>               Deletes bunch.\n"
    "  packbunch add <bunch> <package>...     Adds one or more packages to the bunch.\n"
    "  packbunch remove <bunch> <package>...  Removes one or more packages from the bunch.\n"
    "  packbunch install <bunch>              Installs all packages in bunch.\n"
    "  packbunch uninstall <bunch>            Uninstalls all packages in bunch.\n"
    "  packbunch import <path>                Copies bunch from path into bunch directory.\n"
    "  packbunch export <bunch> <path>        Copies bunch to specified path (must be a directory).\n"
    ;
}

void pb::list()
{
    for (const std::filesystem::directory_entry& file : std::filesystem::directory_iterator {pb::path})
    {
        std::cout << file.path().filename().string() << ' ';
    }
    std::cout << '\n';
}

int pb::view_bunch(const std::string& bunch_name)
{
    if (!pb::valid_bunch_name(bunch_name))
    {
        std::cerr << "Bunch name \"" << bunch_name << "\" is invalid. It can only contain letters, digits, and the following characters: \"_\", \"-\", \".\".\n";
        return pb::FAILURE;
    }

    if (!std::filesystem::exists(std::filesystem::path {pb::path + bunch_name}))
    {
        std::cerr << "Bunch \"" << bunch_name << "\" doesn't exist.\n";
        return pb::FAILURE;
    }

    std::ifstream ifile {pb::path + bunch_name};
    if (!ifile)
    {
        std::cerr << "Couldn't open bunch \"" << bunch_name << "\".\n";
        return pb::FAILURE;
    }
    std::string package {};
    while (ifile >> package)
    {
        std::cout << package << ' ';
    }
    std::cout << '\n';
    return pb::SUCCESS;
}

int pb::create_bunch(const std::string& bunch_name)
{   
    if (!pb::valid_bunch_name(bunch_name))
    {
        std::cerr << "Bunch name \"" << bunch_name << "\" is invalid. It can only contain letters, digits, and the following characters: \"_\", \"-\", \".\".\n";
        return pb::FAILURE;
    }

    if (std::filesystem::exists(std::filesystem::path{pb::path + bunch_name}))
    {
        std::cerr << "Bunch \"" << bunch_name << "\" already exists.\n";
        return pb::FAILURE;
    }

    std::ofstream file {pb::path + bunch_name};
    if (!file)
    {
        std::cerr << "Couldn't create bunch \"" << bunch_name << "\".\n";
        return pb::FAILURE;
    }
    std::cout << "Created bunch \"" << bunch_name << "\".\n";

    return pb::SUCCESS;
}

int pb::delete_bunch(const std::string& bunch_name)
{
    if (!std::filesystem::exists(std::filesystem::path {pb::path + bunch_name}))
    {
        std::cerr << "Bunch \"" << bunch_name << "\" doesn't exist.\n";
        return pb::FAILURE;
    }
    if (std::remove((pb::path + bunch_name).c_str()) != 0)
    {
        std::cerr << "Couldn't delete bunch \"" << bunch_name << "\".\n";
        return pb::FAILURE;
    }

    std::cout << "Deleted bunch \"" << bunch_name << "\".\n";
    return pb::SUCCESS;
}

int pb::add_package(const std::string& bunch_name, const std::string& package_name)
{
    if (!pb::valid_bunch_name(bunch_name))
    {
        std::cerr << "Bunch name \"" << bunch_name << "\" is invalid. It can only contain letters, digits, and the following characters: \"_\", \"-\", \".\".\n";
        return pb::FAILURE;
    }

    if (!pb::valid_package_name(package_name))
    {
        std::cerr << "Package name \"" << package_name << "\" is invalid. It can only contain lowercase letters, digits, and the following characters: \"+\", \"-\", \".\".\n";
        return pb::FAILURE;
    }

    if (!std::filesystem::exists(std::filesystem::path {pb::path + bunch_name}))
    {
        std::cerr << "Bunch \"" << bunch_name << "\" doesn't exist.\n";
        return pb::FAILURE;
    }

    std::ifstream ifile {pb::path + bunch_name};
    if (!ifile)
    {
        std::cerr << "Couldn't open bunch \"" << bunch_name << "\".\n";
        return pb::FAILURE;
    }
    std::vector<std::string> packages {};
    {
        std::string package {};
        while (ifile >> package)
        {
            if (package == package_name)
            {
                std::cerr << "Bunch \"" << bunch_name << "\" already contains package \"" << package_name << "\".\n";
                return pb::FAILURE;
            }
            packages.emplace_back(std::string {package});
        }
    }
    ifile.close();

    std::ofstream ofile {pb::path + bunch_name};
    if (!ofile)
    {
        std::cerr << "Couldn't open bunch \"" << bunch_name << "\".\n";
        return pb::FAILURE;
    }
    for (const std::string& p : packages)
        ofile << p << ' ';
    ofile << package_name << ' ';
    
    std::cout << "Added package \"" << package_name << "\" to bunch \"" << bunch_name << "\".\n";
    return pb::SUCCESS;
}

int pb::remove_package(const std::string& bunch_name, const std::string& package_name)
{
    if (!std::filesystem::exists(std::filesystem::path {pb::path + bunch_name}))
    {
        std::cerr << "Bunch \"" << bunch_name << "\" doesn't exist.\n";
        return pb::FAILURE;
    }

    std::ifstream ifile {pb::path + bunch_name};
    if (!ifile)
    {
        std::cerr << "Couldn't open bunch \"" << bunch_name << "\".\n";
        return pb::FAILURE;
    }
    bool found = false;
    std::vector<std::string> packages {};
    {
        std::string package {};
        while (ifile >> package)
        {
            if (package == package_name)
                found = true;
            else
                packages.emplace_back(std::string {package});
        }
    }
    ifile.close();

    if (!found)
    {
        std::cerr << "Bunch \"" << bunch_name << "\" doesn't contain package \"" << package_name << "\".\n";
        return pb::FAILURE;
    }

    std::ofstream ofile {pb::path + bunch_name};
    if (!ofile)
    {
        std::cerr << "Couldn't open bunch \"" << bunch_name << "\".\n";
        return pb::FAILURE;
    }
    for (const std::string& package : packages)
        ofile << package << ' ';
        
    std::cout << "Removed package \"" << package_name << "\" from bunch \"" << bunch_name << "\".\n";
    return pb::SUCCESS;
}

int pb::install_bunch(const std::string& bunch_name)
{
    if (!pb::valid_bunch_name(bunch_name))
    {
        std::cerr << "Bunch name \"" << bunch_name << "\" is invalid. It can only contain letters, digits, and the following characters: \"_\", \"-\", \".\".\n";
        return pb::FAILURE;
    }

    if (!std::filesystem::exists(std::filesystem::path {pb::path + bunch_name}))
    {
        std::cerr << "Bunch \"" << bunch_name << "\" doesn't exist.\n";
        return pb::FAILURE;
    }

    std::ifstream file {pb::path + bunch_name};
    if (!file)
    {
        std::cerr << "Couldn't open bunch \"" << bunch_name << "\".\n";
        return pb::FAILURE;
    }

    int status = pb::SUCCESS;
    std::string package {};
    while (file >> package)
    {
        if (status == pb::SUCCESS)
        {
            if (pb::valid_package_name(package))
            {
                if (std::system(("apt install " + package).c_str()) == 0)
                {
                    std::cout << "Installed package \"" << package << "\" from bunch \"" << bunch_name << "\".\n";
                }
                else
                {
                    std::cerr << "Couldn't install package \"" << package << "\" from bunch \"" << bunch_name << "\".\n";
                    status = pb::FAILURE;
                }
            }
            else
            {
                std::cerr << "Package name \"" << package << "\" is invalid. It can only contain lowercase letters, digits, and the following characters: \"+\", \"-\", \".\".\n";
                status = pb::FAILURE;
            }
        }
    }

    if (status == pb::SUCCESS)
    {
        std::cout << "Installed bunch \"" << bunch_name << "\".\n";
        return pb::SUCCESS;
    }
    else
    {
        pb::uninstall_bunch(bunch_name);
        std::cerr << "Couldn't install bunch \"" << bunch_name << "\". All changes have been reverted.\n";
        return pb::FAILURE;
    }
}

int pb::uninstall_bunch(const std::string& bunch_name)
{
    if (!pb::valid_bunch_name(bunch_name))
    {
        std::cerr << "Bunch name \"" << bunch_name << "\" is invalid. It can only contain letters, digits, and the following characters: \"_\", \"-\", \".\".\n";
        return pb::FAILURE;
    }

    if (!std::filesystem::exists(std::filesystem::path {pb::path + bunch_name}))
    {
        std::cerr << "Bunch \"" << bunch_name << "\" doesn't exist.\n";
        return pb::FAILURE;
    }

    std::ifstream file {pb::path + bunch_name};
    if (!file)
    {
        std::cerr << "Couldn't open bunch \"" << bunch_name << "\".\n";
        return pb::FAILURE;
    }

    int status = pb::SUCCESS;
    std::string package {};
    while (file >> package)
    {
        if (pb::valid_package_name(package))
        {
            if (std::system(("apt remove " + package).c_str()) == 0)
            {
                std::cout << "Uninstalled package \"" << package << "\" from bunch \"" << bunch_name << "\".\n";
            }
            else
            {
                std::cerr << "Couldn't uninstall package \"" << package << "\" from bunch \"" << bunch_name << "\".\n";
                status = pb::FAILURE;
            }
        }
    }

    std::cout << "Uninstalled bunch \"" << bunch_name << "\".\n";
    return status;
}

int pb::import_bunch(const std::string& bunch_path)
{
    std::filesystem::path path {bunch_path};

    if (!std::filesystem::exists(path))
    {
        std::cerr << "No bunch found at \"" << bunch_path << "\".\n";
        return pb::FAILURE;
    }

    std::string bunch_name {path.filename().string()};

    if (!pb::valid_bunch_name(bunch_name))
    {
        std::cerr << "Bunch name \"" << bunch_name << "\" is invalid. It can only contain letters, digits, and the following characters: \"_\", \"-\", \".\".\n";
        return pb::FAILURE;
    }

    if (std::filesystem::exists(std::filesystem::path {pb::path + bunch_name}))
    {
        std::cerr << "Bunch \"" << bunch_name << "\" already exists.\n";
        return pb::FAILURE;
    }

    if (std::filesystem::copy_file(bunch_path, pb::path + bunch_name))
    {
        std::cout << "Imported bunch \"" << bunch_name << "\".\n";
        return pb::SUCCESS;
    }
    else
    {
        std::cerr << "Couldn't import bunch \"" << bunch_name << "\".\n";
        return pb::FAILURE;
    }
}

int pb::export_bunch(const std::string& bunch_name, const std::string& export_path)
{
    if (!pb::valid_bunch_name(bunch_name))
    {
        std::cerr << "Bunch name \"" << bunch_name << "\" is invalid. It can only contain letters, digits, and the following characters: \"_\", \"-\", \".\".\n";
        return pb::FAILURE;
    }

    if (!std::filesystem::exists(std::filesystem::path {pb::path + bunch_name}))
    {
        std::cerr << "Bunch \"" << bunch_name << "\" doesn't exist.\n";
        return pb::FAILURE;
    }

    std::filesystem::path dir_path {export_path};

    if (std::filesystem::exists(dir_path))
    {
        if (std::filesystem::is_directory(dir_path))
        {
            std::filesystem::path final_path {export_path + bunch_name};
            if (std::filesystem::exists(final_path))
            {
                std::cerr << "File \"" << final_path.string() << "\" already exists.\n";
                return pb::FAILURE;
            }
            if (std::filesystem::copy_file(pb::path + bunch_name, final_path))
            {
                std::cout << "Exported bunch \"" << bunch_name << "\" to \"" << export_path << "\".\n";
                return pb::SUCCESS;
            }
            else
            {
                std::cerr << "Couldn't export bunch \"" << bunch_name << "\".\n";
                return pb::FAILURE;
            }
        }
        else
        {
            std::cerr << "Path \"" << export_path << "\" isn't a directory.\n";
            return pb::FAILURE;
        }
    }
    else
    {
        std::cerr << "Directory \"" << export_path << "\" doesn't exist.\n";
        return pb::FAILURE;
    }
}

bool pb::valid_bunch_name(const std::string& name)
{
    bool valid = true;
    for (const char& c : name)
    {
        if (!std::isalnum(c))
        {
            switch (c)
            {
            case '_':
                break;
            case '-':
                break;
            case '.':
                break;
            default:
                valid = false;
                break;
            }
        }
    }
    return valid;
}

bool pb::valid_package_name(const std::string& bunch_name)
{
    bool valid = true;
    for (const char& c : bunch_name)
    {
        if (std::isalnum(c))
        {
            if (std::isupper(c))
                valid = false;
        }
        else
        {
            switch (c)
            {
            case '+':
                break;
            case '-':
                break;
            case '.':
                break;
            default:
                valid = false;
                break;
            }
        }
    }
    return valid;
}