using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.Assertions;

public class PlayerController : MonoBehaviour, ISavableDataContainer<PlayerDataSerializable>
{
    public static PlayerController Instance { get; private set; }
    [SerializeField] private List<Robot> _robots;
    public Robot ActiveRobot { get; private set; }
    private GameDataSaver<PlayerController, PlayerDataSerializable> _saver;

    private void Awake()
    {
        DontDestroyOnLoad(gameObject);
        if (Instance == null)
        {
            Instance = this;
        }
        if (Instance != this)
        {
            Destroy(gameObject);
            return;
        }
        Assert.IsNotNull(_robots);
        Assert.IsFalse(_robots.Count == 0);
        _saver = new GameDataSaver<PlayerController, PlayerDataSerializable>(Instance, SaveIds.PlayerData);
        _saver.Load();
        if (ActiveRobot == null)
        {
            // save file doesn't exist or is invalid
            ActiveRobot = GetDefaultRobot();
        }
    }

    private void OnEnable()
    {
        GarageController.RobotChanged += HandleRobotChange;
    }

    private void OnDisable()
    {
        GarageController.RobotChanged -= HandleRobotChange;
    }

    private void HandleRobotChange(Robot robot)
    {
        if (robot.Equals(ActiveRobot))
            return;
        if (robot.PlayerData.ownedByPlayer)
        {
            ActiveRobot = _robots.Where(r => r.Equals(robot)).First();
            _saver.Save();
        }
    }

    public PlayerDataSerializable GetSerializable()
    {
        return new PlayerDataSerializable
        {
            activeRobotName = ActiveRobot.defaultData.unitName
        };
    }

    public void Deserialize(PlayerDataSerializable data)
    {
        for (int i = 0; i < _robots.Count; i++)
        {
            if (data.activeRobotName == _robots[i].defaultData.unitName)
            {
                ActiveRobot = _robots[i];
                break;
            }
        }
    }

    private Robot GetDefaultRobot()
    {
        return _robots.Where(r => r.defaultData.ownedByPlayer).First();
    }
}
