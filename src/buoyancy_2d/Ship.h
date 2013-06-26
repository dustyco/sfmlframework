



#include <SFML/Graphics.hpp>


struct Ship : public sf::Drawable, public sf::Transformable {
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		states.transform *= getTransform();
		target.draw(..., states);
	}
};
