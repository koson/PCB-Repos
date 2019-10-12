typedef struct segment_search{
	int nesw = 0;//0 - |__|, 1 - | |, 2 - |‾‾, 3 - ‾‾|, 4 -   |, 5 - |   , 6 - |‾‾|
	float time;//time driven in this segment
	struct segment_search * next;
}segment_search_t;

segment_search_t maze_array[16][16];


void DriveMiddle(float power_percentR, float power_percentL)
{
	adcRLerror = adc1 - adc2; //positive gives right offset(right side too close)
	abs_adcRLerror = abs(adcRLerror);
	
	if((adc0 < SensorStraightMax) && (adc3 < SensorStraightMax)){
		active_power_percentR = max_power_percent;
		active_power_percentL = max_power_percent;
		//update_segment = last_segment;
	}

	if((adc0 > SensorStraightMax) && (adc3 > SensorStraightMax) && (adc1 < SensorSideMin) && (adc2 > SensorSideMin)){
		Nextegment(2);
		MakeTurnRight();
		//MakeTurn();
	}else if if((adc0 > SensorStraightMax) && (adc3 > SensorStraightMax) && (adc1 > SensorSideMin) && (adc2 < SensorSideMin)){
		Nextegment(3);
		MakeTurnLeft();
		//MakeTurn();
	}
	else if((adc0 > SensorStraightMax) && (adc3 > SensorStraightMax)){
		RightHardStop(TIM2_Data);
		LeftHardStop(TIM3_Data);
		//MakeTurn();
	}
	if((adcRLerror > 0) && (adcRLerror > MinError)){
		RightForward(TIM2_Data, active_power_percentR + abs_adcRLerror);
		LeftForward(TIM3_Data, active_power_percentL - abs_adcRLerror);
	}else if((adcRLerror < 0) && (abs_adcRLerror > MinError)){
		RightForward(TIM2_Data, active_power_percentR - abs_adcRLerror);
		LeftForward(TIM3_Data, active_power_percentL + abs_adcRLerror);
	}else if((abs_adcRLerror <= MinError)){
		RightForward(TIM2_Data, active_power_percentR);
		LeftForward(TIM3_Data, active_power_percentL);
	}
}




void AddSegment(struct segment_search** pointer1, const int *nesw, const int *time)
{
    struct plate_number_list *p1 = malloc(sizeof(*p1));
    strcpy(p1->tl, tab1);
    p1->tmp[0] = 0;
    p1->next = NULL;
    while (*pointer1)
        pointer1 = &(*pointer1)->next;
        *pointer1 = p1;

    struct plate_number_list *p2 = malloc(sizeof(*p2));
    strcpy(p2->tn, tab2);
    p2->tmp[0] = 0;
    p2->next = NULL;
    while (*pointer2)
        pointer2 = &(*pointer2)->next;
        *pointer2 = p2;
}

int NextSegment(float nesw){
	if(adc){

	}
}

if(distance == segment_lenght) {
		if(update_segment){
			NextSegment();
		}else{
			NextSegment(PreviousSegment);
			distance  = 0;
		}
	}