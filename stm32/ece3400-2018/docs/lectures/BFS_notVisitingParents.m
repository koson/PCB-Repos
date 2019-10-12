%Breadth first search in ECE3400 maze

clear all;
mazeSize = 9;
init.loc = [5 5];
init.par = init.loc;
init.depth = 1;

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
        if((n.loc(2)>1) && ~all(n.par == [n.loc(1) n.loc(2)-1])) %North and not parent
            neighbor.loc = [n.loc(1) n.loc(2)-1];
            neighbor.par = n.loc;
            neighbor.depth = n.depth+1;
            frontier = [frontier; neighbor];
        end
        if((n.loc(1)<mazeSize && ~all(n.par == [n.loc(1)+1 n.loc(2)]))) %East and not parent
            neighbor.loc = [n.loc(1)+1 n.loc(2)];
            neighbor.par = n.loc;
            neighbor.depth = n.depth+1;
            frontier = [frontier; neighbor];
        end
        if((n.loc(2)<mazeSize) && ~all(n.par == [n.loc(1) n.loc(2)+1])) %South and not parent
            neighbor.loc = [n.loc(1) n.loc(2)+1];
            neighbor.par = n.loc;
            neighbor.depth = n.depth+1;
            frontier = [frontier; neighbor];
        end
        if((n.loc(1)>1) && ~all(n.par == [n.loc(1)-1 n.loc(2)])) %West and not parent
            neighbor.loc = [n.loc(1)-1 n.loc(2)];
            neighbor.par = n.loc;
            neighbor.depth = n.depth+1;
            frontier = [frontier; neighbor];
        end
        display(['Depth: ' int2str(n.depth+1) ', frontier size: ' int2str(size(frontier,1))]);
        debug(n.depth) = size(frontier,1);
        if(size(frontier,1)>10000), break; end %Extra stop condition
    end
end

%Plotting stuff:
plot(debug(),'LineWidth',2)
title('BFS: Growth of frontier');
xlabel('depth');
ylabel('nodes explored');
set(gca,'FontSize', 14);
