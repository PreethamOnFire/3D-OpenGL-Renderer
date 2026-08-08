#pragma once

// Base class for individual ImGui panels (Scene, Inspector, Lighting, Materials, Performance, ...).
// UIManager owns one instance per panel and calls draw() each frame when the panel is visible.
class UIPanel {
public:
    virtual ~UIPanel() = default;

    // Draw this panel's ImGui window. Called once per frame between
    // ImGui::NewFrame() and ImGui::Render() while visible is true.
    virtual void draw() = 0;

    virtual const char* getName() const = 0;

    bool isVisible() const { return visible; }
    void setVisible(bool v) { visible = v; }

protected:
    bool visible = true;
};
