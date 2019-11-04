#include "components/RatingComponent.h"

#include "resources/TextureResource.h"
#include "ThemeData.h"

RatingComponent::RatingComponent(Window* window) : GuiComponent(window), starFilled(window), starUnfilled(window)
{
	starFilled.setImage(":/star_filled.svg");
	starUnfilled.setImage(":/star_unfilled.svg");
	mValue = 0.5f;
	starFilled.setResize(64,64);
	starUnfilled.setResize(64,64);
}

void RatingComponent::setValue(const std::string& value)
{
	if(value.empty())
	{
		mValue = 0.0f;
	}else{
		mValue = stof(value);
		if(mValue > 1.0f)
			mValue = 1.0f;
		else if(mValue < 0.0f)
			mValue = 0.0f;
	}
}

std::string RatingComponent::getValue() const
{
	// do not use std::to_string here as it will use the current locale
	// and that sometimes encodes decimals as commas
	std::stringstream ss;
	ss << mValue;
	return ss.str();
}

void RatingComponent::setOpacity(unsigned char opacity)
{
	starFilled.setOpacity(opacity);
	starUnfilled.setOpacity(opacity);
}

void RatingComponent::setColorShift(unsigned int color)
{
	starFilled.setColorShift(color);
	starUnfilled.setColorShift(color);
}

void RatingComponent::onSizeChanged()
{
	if(mSize.y() == 0)
		mSize[1] = mSize.x() / NUM_RATING_STARS;
	else if(mSize.x() == 0)
		mSize[0] = mSize.y() * NUM_RATING_STARS;

	if(mSize.y() > 0)
	{
		size_t heightPx = (size_t)Math::round(mSize.y());
		starFilled.setResize(heightPx, heightPx);
		starUnfilled.setResize(heightPx, heightPx);
	}
}

void RatingComponent::render(const Transform4x4f& parentTrans)
{
	if (!isVisible())
		return;

	Transform4x4f trans = parentTrans * getTransform();
	trans.round();
	
	for(int i = 0, starCount=mValue*NUM_RATING_STARS; i < NUM_RATING_STARS; ++i)
	{
		if(i < starCount)
			starFilled.render(trans);
		starUnfilled.render(trans);
		trans.translate(Vector3f(starFilled.getSize().x(), 0.f, 0.f));
	}

	renderChildren(trans);
}

bool RatingComponent::input(InputConfig* config, Input input)
{
	if(config->isMappedTo("a", input) && input.value != 0)
	{
		mValue += 1.f / NUM_RATING_STARS;
		if(mValue > 1.0f)
			mValue = 0.0f;
	}

	return GuiComponent::input(config, input);
}

void RatingComponent::applyTheme(const std::shared_ptr<ThemeData>& theme, const std::string& view, const std::string& element, unsigned int properties)
{
	GuiComponent::applyTheme(theme, view, element, properties);

	using namespace ThemeFlags;

	const ThemeData::ThemeElement* elem = theme->getElement(view, element, "rating");
	if(!elem)
		return;

	bool imgChanged = false;
	if(properties & PATH && elem->has("filledPath"))
	{
		starFilled.setImage(elem->get<std::string>("filledPath"));
		imgChanged = true;
	}
	if(properties & PATH && elem->has("unfilledPath"))
	{
		starUnfilled.setImage(elem->get<std::string>("unfilledPath"));
		imgChanged = true;
	}


	if(properties & COLOR && elem->has("color"))
		setColorShift(elem->get<unsigned int>("color"));

	if(imgChanged)
		onSizeChanged();
}

std::vector<HelpPrompt> RatingComponent::getHelpPrompts()
{
	std::vector<HelpPrompt> prompts;
	prompts.push_back(HelpPrompt("a", "add star"));
	return prompts;
}
