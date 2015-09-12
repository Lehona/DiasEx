#include "bfeFile.h"

#include <iostream>
#include <fstream>

using namespace bfe;

File::File(const std::string &path) : path{path}
{
  auto file = std::basic_ifstream<char>{path, std::ios::in | std::ios::binary};
	file.unsetf(std::ios::skipws);

	if(!file.is_open()) {
		valid = false;
		return;
	}

	file.seekg(0, std::ios::end);
	auto fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	bytes.reserve(size_t(fileSize));
	bytes.insert(std::begin(bytes), std::istreambuf_iterator<char>{file}, {});

	valid = file.good();
}

bool File::write(const std::string &file, const std::vector<char> &stream)
{
	std::ofstream os(file, std::ios::out | std::ios::binary);

	if(!os.is_open())
		return false;

	os.write(&stream[0], stream.size());
	os.close();

	return true;
}

Folder::Folder(const std::string &folder) : folder(folder)
{
	validatePath();

	DWORD attr = GetFileAttributesA(folder.c_str());
	if(attr == INVALID_FILE_ATTRIBUTES)
		valid = false;
	else if(attr & FILE_ATTRIBUTE_DIRECTORY)
		valid = true;
	else
		valid = false;
}

bool Folder::exists() const { return valid; }

const Folder Folder::getFolder(const std::string &sub) const
{
	//if(!valid) error("trying to use methods on non-existing folder " + folder);
	return Folder(folder + sub);
}

const std::vector<Folder> Folder::getAllFolders() const
{
	std::vector<Folder> ret;

	foreach(folder + "*", [&](WIN32_FIND_DATA &fd) {
		if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			std::string fname(fd.cFileName);
			if(fname.compare(".") && fname.compare(".."))
				ret.push_back(Folder(folder + fname + "/"));
		}
	})
		;

	return ret;
}

const std::vector<std::string> Folder::getAllFiles() const
{
	std::vector<std::string> ret;

	foreach(folder + "*", [&](WIN32_FIND_DATA &fd) {
		if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			ret.push_back(folder + std::string(fd.cFileName));
		}
	})
		;

	return ret;
}

const std::vector<std::string> Folder::getAllFilesRecursive() const
{
	std::vector<std::string> ret;

	getAllFilesRecursive(ret);

	return ret;
}

void Folder::getAllFilesRecursive(std::vector<std::string> &ret) const
{
	for(Folder f : getAllFolders()) {
		f.getAllFilesRecursive(ret);
	}

	foreach(folder + "*", [&](WIN32_FIND_DATA &fd) {
		if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			// info("fetching " + folder + std::string(fd.cFileName));
			ret.push_back(folder + std::string(fd.cFileName));
		}
	})
		;
}

const std::vector<std::string>
Folder::getAllFileNames(const std::string &ending) const
{
	std::vector<std::string> ret;

	foreach(folder + "*" + ending, [&](WIN32_FIND_DATA &fd) {
		if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			std::string fname(fd.cFileName);
			ret.push_back(fname.substr(0, fname.size() - ending.size()));
		}
	})
		;

	return ret;
}

void Folder::foreach(const std::string &filter,
                     const std::function<void(WIN32_FIND_DATA &)> &func) const
{
	//if(!valid) error("trying to use methods on non-existing folder " + folder);

	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(filter.c_str(), &fd);

	if(hFind == INVALID_HANDLE_VALUE)
		return;

	do {
		func(fd);
	} while(FindNextFile(hFind, &fd));

	FindClose(hFind);
}

void Folder::validatePath()
{
	if(folder.back() != '/')
		folder += '/';
}