#pragma once

#include <string>
#include <vector>
#include <functional>

#define NOMINMAX
#include <Windows.h>

namespace bfe {
	class File {
	public:
		std::vector<char> bytes;
		std::string path;

		bool valid = false;

		File(const std::string &path);
        
		static bool write(const std::string &file, const std::vector<char> &stream);
	};

	class Folder {
	public:
		Folder(const std::string &folder);

		bool exists() const;

		const Folder getFolder(const std::string &subfolder) const;

		const std::vector<Folder> getAllFolders() const;
		const std::vector<std::string> getAllFiles() const;
		const std::vector<std::string> getAllFilesRecursive() const;
		const std::vector<std::string>
		getAllFileNames(const std::string &ending) const;

		void foreach(const std::string &filter,
		             const std::function<void(WIN32_FIND_DATA &)> &func) const;

	private:
		void validatePath();

		void getAllFilesRecursive(std::vector<std::string> &ret) const;

		std::string folder;
		bool valid;
	};
}