#pragma once


class View : public juce::Component
{
public:
    View();
    ~View();
    
    void paint(juce::Graphics& g) override;
    void resized() override; 

private:
    const int m_numBoxes = 108 - 21;
    juce::OwnedArray<NoteBox> m_noteBoxes;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(View)
};
