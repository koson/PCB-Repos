%Breadth first search in ECE3400 maze

clear all;
mazeSize = 9;
init.loc = [5 5];
init.par = init.loc;
init.depth = 1;
goal.loc = [mazeSize mazeSize];

visited = [];
frontier = init;
j=1;
while(size(frontier,1)>0)
    visited = [visited; frontier];
    nextfrontier = expand_level(frontier,visited,mazeSize);
    if(checkVisited(visited,goal))
        display('Goal!');
        break;
    end
    j=j+1;
    display(['Depth: ' int2str(j) ', frontier size: ' int2str(size(frontier,1))]);
    frontier = nextfrontier;
end

%Check if node location is in array of visited locations
function answer = checkVisited(array,node)
    answer = 0;
    for(j=1:size(array,1))
        if(array(j).loc(1)==node.loc(1) && array(j).loc(2)==node.loc(2))
            answer = 1;
        end
    end
end
 
function nextlevel = expand_level(level,visited,mazeSize)
    nextlevel = [];
    for(i=1:size(level,1))
        n = level(i);
        %for all actions in n
        if(n.loc(2)>1 && ~all(n.par == [n.loc(1) n.loc(2)-1])) %North && not parent
            neighbor.loc = [n.loc(1) n.loc(2)-1];
            neighbor.par = n.loc;
            neighbor.depth = n.depth+1;
            if(~checkVisited(visited,neighbor))
                nextlevel = [nextlevel; neighbor];
            end
        end
        if((n.loc(1)<mazeSize) && ~all(n.par == [n.loc(1)+1 n.loc(2)])) %East && not parent
            neighbor.loc = [n.loc(1)+1 n.loc(2)];
            neighbor.par = n.loc;
            neighbor.depth = n.depth+1;
            if(~checkVisited(visited,neighbor))
                nextlevel = [nextlevel; neighbor];
            end
        end
        if(n.loc(2)<mazeSize && ~all(n.par == [n.loc(1) n.loc(2)+1])) %South && not parent
            neighbor.loc = [n.loc(1) n.loc(2)+1];
            neighbor.par = n.loc;
            neighbor.depth = n.depth+1;
            if(~checkVisited(visited,neighbor))
                nextlevel = [nextlevel; neighbor];
            end
        end
        if(n.loc(1)>1 && ~all(n.par == [n.loc(1)-1 n.loc(2)])) %West && not parent
            neighbor.loc = [n.loc(1)-1 n.loc(2)];
            neighbor.par = n.loc;
            neighbor.depth = n.depth+1;
            if(~checkVisited(visited,neighbor))
                nextlevel = [nextlevel; neighbor];
            end
        end        
    end
end