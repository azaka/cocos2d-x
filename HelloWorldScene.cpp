#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
    closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
    
    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 10);

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(sprite, 0);
    

	/**********************************************************************************

	SpriteStudio�A�j���[�V�����\���̃T���v���R�[�h
	Visual Studio Community 2017�œ�����m�F���Ă��܂��B

	ssbp��png������΍Đ����鎖���ł��܂����AResources�t�H���_��sspj���܂܂�Ă��܂��B

	**********************************************************************************/

	//�v���C���[���g�p����O�̏���������
	//���̏����̓A�v���P�[�V�����̏������łP�x�����s���Ă��������B
	ss::SSPlatformInit();
	//�v���C���[���g�p����O�̏��������������܂�


	//���\�[�X�}�l�[�W���̍쐬
	resman = ss::ResourceManager::getInstance();
	
	//SS6Player for Cocos2d-x�ł�SSPlayerControl���쐬���AgetSSPInstance()���o�R���ăv���C���[�𑀍삵�܂�
	//�v���C���[�̍쐬
	ssplayer = ss::SSPlayerControl::create();
	//�A�j���f�[�^�����\�[�X�ɒǉ�
	//���ꂼ��̃v���b�g�t�H�[���ɍ��킹���p�X�֕ύX���Ă��������B
	resman->addData("character_template_comipo/character_template1.ssbp");
	//�v���C���[�Ƀ��\�[�X�����蓖��
	ssplayer->getSSPInstance()->setData("character_template1");        // ssbp�t�@�C�����i�g���q�s�v�j
	 //�Đ����郂�[�V������ݒ�
	ssplayer->getSSPInstance()->play("character_template_3head/stance");				 // �A�j���[�V���������w��(ssae��/�A�j���[�V������)


	//�\���ʒu��ݒ�
	Size size = cocos2d::Director::getInstance()->getWinSize();
	ssplayer->setPosition(size.width / 2, size.height / 2);
	ssplayer->setScale(1.0f, 1.0f);
	ssplayer->setRotation(0);
	ssplayer->setOpacity(255);
	ssplayer->getSSPInstance()->setColor(255, 255, 255);
	ssplayer->getSSPInstance()->setFlip(false, false);

	//���[�U�[�f�[�^�R�[���o�b�N��ݒ�
	ssplayer->getSSPInstance()->setUserDataCallback(CC_CALLBACK_2(HelloWorld::userDataCallback, this));

	//�A�j���[�V�����I���R�[���o�b�N��ݒ�
	ssplayer->getSSPInstance()->setPlayEndCallback(CC_CALLBACK_1(HelloWorld::playEndCallback, this));

	//�v���C���[���Q�[���V�[���ɒǉ�
	this->addChild(ssplayer, 1);

	//ssbp�Ɋ܂܂�Ă���A�j���[�V�������̃��X�g���擾����
	animename = resman->getAnimeName(ssplayer->getSSPInstance()->getPlayDataName());
	playindex = 0;				//���ݍĐ����Ă���A�j���̃C���f�b�N�X
	playerstate = 0;

	//�L�[�{�[�h���̓R�[���o�b�N�̍쐬
	auto listener = cocos2d::EventListenerKeyboard::create();
	listener->onKeyPressed = CC_CALLBACK_2(HelloWorld::onKeyPressed, this);
	listener->onKeyReleased = CC_CALLBACK_2(HelloWorld::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


	//updete�̍쐬
	this->scheduleUpdate();

    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

	ss:: SSPlatformRelese();


    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
    
    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/
    
    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
    
    
}

void HelloWorld::update(float delta)
{
	int max_frame = 0;
	int now_frame = 0;
	if (ssplayer)
	{
		max_frame = ssplayer->getSSPInstance()->getEndFrame();
		now_frame = ssplayer->getSSPInstance()->getFrameNo();
	}

	{
		auto str = String::createWithFormat("max:%d, fream:%d", max_frame, now_frame);
		label->setString(str->getCString());
	}
}

//���[�U�[�f�[�^�R�[���o�b�N
void HelloWorld::userDataCallback(ss::Player* player, const ss::UserData* data)
{
	//�Đ������t���[���Ƀ��[�U�[�f�[�^���ݒ肳��Ă���ꍇ�Ăяo����܂��B
	//�v���C���[�𔻒肷��ꍇ�A�Q�[�����ŊǗ����Ă���ss::Player�̃A�h���X�Ɣ�r���Ĕ��肵�Ă��������B
	/*
	//�R�[���o�b�N���Ńp�[�c�̃X�e�[�^�X���擾�������ꍇ�́A���̎��_�ł̓A�j�����X�V����Ă��Ȃ����߁A
	//getPartState�@�Ɂ@data->frameNo�@�Ńt���[�������w�肵�Ď擾���Ă��������B
	ss::ResluteState result;
	//�Đ����Ă��郂�[�V�����Ɋ܂܂��p�[�c���ucollision�v�̃X�e�[�^�X���擾���܂��B
	ssplayer->getPartState(result, "collision", data->frameNo);
	*/

}

//�A�j���[�V�����I���R�[���o�b�N
void HelloWorld::playEndCallback(ss::Player* player)
{
	//�Đ������A�j���[�V�������I�������i�K�ŌĂяo����܂��B
	//�v���C���[�𔻒肷��ꍇ�A�Q�[�����ŊǗ����Ă���ss::Player�̃A�h���X�Ɣ�r���Ĕ��肵�Ă��������B
	//player->getPlayAnimeName();
	//���g�p���鎖�ōĐ����Ă���A�j���[�V���������擾���鎖���ł��܂��B

	//���[�v�񐔕��Đ�������ɌĂяo�����_�ɒ��ӂ��Ă��������B
	//�������[�v�ōĐ����Ă���ꍇ�̓R�[���o�b�N���������܂���B

}

// �L�[�{�[�h���̓R�[���o�b�N
bool press = false;
void HelloWorld::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	log("Key with keycode %d pressed", keyCode);
	if(press == false)
	{ 
		if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW)
		{
			playindex--;
			if (playindex < 0)
			{
				playindex = animename.size() - 1;
			}
			std::string name = animename.at(playindex);
			ssplayer->getSSPInstance()->play(name);
		}
		else if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
		{
			playindex++;
			if (playindex >= animename.size())
			{
				playindex = 0;
			}
			std::string name = animename.at(playindex);
			ssplayer->getSSPInstance()->play(name);
		}
		if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_Z)
		{
			if (playerstate == 0)
			{
				ssplayer->getSSPInstance()->animePause();
				playerstate = 1;
			}
			else
			{
				ssplayer->getSSPInstance()->animeResume();
				playerstate = 0;
			}
		}
	}
	press = true;
}

void HelloWorld::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	log("Key with keycode %d released", keyCode);
	press = false;
}
