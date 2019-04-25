#include <string>

class Normalize{
    private:
      double MinMaxLongitude[2];
      double MinMaxLatitude[2];
      std::string name;
      std::string newName;

      double normalizeLatitude(double latitude);
      double normalizeLongitude(double longitude);
      void checkLong(double longitude);
      void checkLat(double latitude);
      void createNormalizedFile();
      void treatFile();
      void generatePathNames(std::string file_path, std::string file_name, std::string file_extension);

    public:
      Normalize(std::string file_path, std::string file_name, std::string file_extension);
      std::string getOldFilePath();
      std::string getNewFilePath();
};

