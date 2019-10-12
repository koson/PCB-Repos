%Breadth first search in ECE3400 maze

clear all; close all; clc;
mazeSize = 9;
init.loc = [5 5];
init.par = init.loc;
init.depth = 1;

visited = init;
frontier = init;
while(size(frontier,1)>0)
    %Get new frontier
	n = frontier(1); %First-In First-Out Buffer
    frontier(1) = [];
    %At goal
	if(n.loc==[mazeSize,mazeSize])
        display(['Goal! - Largest frontier: ' num2str(max(debug))]); 
        break;
    %Not at goal
    else
        %for all actions in n
        if(n.loc(2)>1) %North
            neighbor.loc = [n.loc(1) n.loc(2)-1];
            neighbor.par = n.loc;
            neighbor.depth = n.depth+1;
            if(~checkVisited(visited,neighbor))
                visited = [visited; neighbor];
                frontier = [frontier; neighbor];
            end
        end
        if(n.loc(1)<mazeSize) %East
            neighbor.loc = [n.loc(1)+1 n.loc(2)];
            neighbor.par = n.loc;
            neighbor.depth = n.depth+1;
            if(~checkVisited(visited,neighbor))
                visited = [visited; neighbor];
                frontier = [frontier; neighbor];
            end
        end
        if(n.loc(2)<mazeSize) %South
            neighbor.loc = [n.loc(1) n.loc(2)+1];
            neighbor.par = n.loc;
            neighbor.depth = n.depth+1;
            if(~checkVisited(visited,neighbor))
                visited = [visited; neighbor];
                frontier = [frontier; neighbor];
            end
        end
        if(n.loc(1)>1) %West
            neighbor.loc = [n.loc(1)-1 n.loc(2)];
            neighbor.par = n.loc;
            neighbor.depth = n.depth+1;
            if(~checkVisited(visited,neighbor))
                visited = [visited; neighbor];
                frontier = [frontier; neighbor];
            end
        end
        display(['Depth: ' int2str(n.depth) ', frontier size: ' int2str(size(frontier,1)-1)]);
        debug(n.depth) = size(frontier,1);
    end
end

%Plotting stuff:
plot(debug()-1,'LineWidth',2)
title('BFS: Growth of frontier');
xlabel('depth');
ylabel('nodes explored');
set(gca,'FontSize', 14);

%Check if node location is in array of visited locations
function answer = checkVisited(array,node)
    answer = 0;
    for(j=1:size(array,1))
        if(array(j).loc(1)==node.loc(1) && array(j).loc(2)==node.loc(2))
            answer = 1;
        end
    end
end
 
