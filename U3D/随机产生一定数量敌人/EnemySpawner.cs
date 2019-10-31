using UnityEngine;
using System.Collections;

public class EnemySpawner : MonoBehaviour {
	public GameObject[] Objectman; // Ememy object
    private int index;

    public string Tag = "Enemy";
	public float timeSpawn = 3;
    private float timetemp = 0;
	public int enemyCount = 10;
	public int radius = 10;
	
		
	void Start () {
		index = Random.Range(0,Objectman.Length);
		timetemp = Time.time;
	}


    public bool Enabled = true;
	void Update () {
        if (!Enabled)
            return;	
		
		var gos	= GameObject.FindGameObjectsWithTag(Tag);
   		if(gos.Length < enemyCount && Time.time > timetemp + timeSpawn)
		{		
 			timetemp = Time.time;
			GameObject obj = (GameObject)GameObject.Instantiate(Objectman[index],transform.position + new Vector3(Random.Range(-radius,radius),0,Random.Range(-radius,radius)),Quaternion.identity);
			obj.tag = Tag;
 			index = Random.Range(0,Objectman.Length);
		}
	}
}
