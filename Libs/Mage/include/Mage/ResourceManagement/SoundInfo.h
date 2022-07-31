#pragma once

namespace Mage
{
    class SoundInfo final
    {
    public:
        SoundInfo(const std::string& filePath, float volume, bool loops);

        const std::string& GetFilePath() const { return m_FilePath; }

    	float GetVolume() const { return m_Volume; }
        void SetVolume(float volume) { m_Volume = volume; }

    	bool IsLoop() const { return m_Loops; }
        void SetLoop(bool loops) { m_Loops = loops; }

        bool IsLoaded() const { return m_IsLoaded; }
        void SetLoaded() { m_IsLoaded = true; }

        int GetUniqueId() const { return m_Id; }

    private:
        std::string m_FilePath;
        float m_Volume;
        bool m_Loops;
        bool m_IsLoaded{ false };

        // Unique id for each sound
        int m_Id;
        static int m_Counter;
    };
}
