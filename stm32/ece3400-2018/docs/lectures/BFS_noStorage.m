%Breadth first search in ECE3400 maze

clear all; close all; clc
mazeSize = 9; 
init.loc = [5 5]; %init location
init.depth = 1; %Depth of graph

frontier = init; %Add init to frontier
while(size(frontier,1)>0)
	n = frontier(1); %Get new frontier: First-In First-Out Buffer
    frontier(1) = [];
	if(n.loc==[mazeSize,mazeSize]) %At goal
        display('Goal!'); 
        break;
    else                           %Not at goal
        %for all actions in n
        if(n.loc(2)>1) %North
            neighbor.loc = [n.loc(1) n.loc(2)-1]; 
            neighbor.depth = n.depth+1;
            frontier = [frontier; neighbor];
        end
        if((n.loc(1)<mazeSize)) %East
            neighbor.loc = [n.loc(1)+1 n.loc(2)];
            neighbor.depth = n.depth+1;
            frontier = [frontier; neighbor];
        end
        if((n.loc(2)<mazeSize)) %South
            neighbor.loc = [n.loc(1) n.loc(2)+1];
            neighbor.depth = n.depth+1;
            frontier = [frontier; neighbor];
        end
        if((n.loc(1)>1)) %West
            neighbor.loc = [n.loc(1)-1 n.loc(2)];
            neighbor.depth = n.depth+1;
            frontier = [frontier; neighbor];
        end
        display(['depth: ' int2str(n.depth) ', frontier size: ' int2str(size(frontier,1))]);
        debug(n.depth) = size(frontier,1);
    end
    if(size(frontier,1)>10000), break; end %Extra stop condition
end

%Plotting stuff:
plot(debug(),'LineWidth',2)
title('BFS: Growth of frontier');
xlabel('depth');
ylabel('nodes explored');
set(gca,'FontSize', 14);
